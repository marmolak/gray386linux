/*
 * Copyright (c) 2008, 2009 open80211s Ltd.
 * Author:     Luis Carlos Cobo <luisca@cozybit.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/gfp.h>
#include <linux/kernel.h>
#include <linux/random.h>
#include "ieee80211_i.h"
#include "rate.h"
#include "mesh.h"

#define PLINK_GET_LLID(p) (p + 2)
#define PLINK_GET_PLID(p) (p + 4)

#define mod_plink_timer(s, t) (mod_timer(&s->plink_timer, \
				jiffies + HZ * t / 1000))

#define dot11MeshMaxRetries(s) (s->u.mesh.mshcfg.dot11MeshMaxRetries)
#define dot11MeshRetryTimeout(s) (s->u.mesh.mshcfg.dot11MeshRetryTimeout)
#define dot11MeshConfirmTimeout(s) (s->u.mesh.mshcfg.dot11MeshConfirmTimeout)
#define dot11MeshHoldingTimeout(s) (s->u.mesh.mshcfg.dot11MeshHoldingTimeout)
#define dot11MeshMaxPeerLinks(s) (s->u.mesh.mshcfg.dot11MeshMaxPeerLinks)

/* We only need a valid sta if user configured a minimum rssi_threshold. */
#define rssi_threshold_check(sta, sdata) \
		(sdata->u.mesh.mshcfg.rssi_threshold == 0 ||\
		(sta && (s8) -ewma_read(&sta->avg_signal) > \
		sdata->u.mesh.mshcfg.rssi_threshold))

enum plink_event {
	PLINK_UNDEFINED,
	OPN_ACPT,
	OPN_RJCT,
	OPN_IGNR,
	CNF_ACPT,
	CNF_RJCT,
	CNF_IGNR,
	CLS_ACPT,
	CLS_IGNR
};

static int mesh_plink_frame_tx(struct ieee80211_sub_if_data *sdata,
		enum ieee80211_self_protected_actioncode action,
		u8 *da, __le16 llid, __le16 plid, __le16 reason);

static inline
u32 mesh_plink_inc_estab_count(struct ieee80211_sub_if_data *sdata)
{
	atomic_inc(&sdata->u.mesh.mshstats.estab_plinks);
	return mesh_accept_plinks_update(sdata);
}

static inline
u32 mesh_plink_dec_estab_count(struct ieee80211_sub_if_data *sdata)
{
	atomic_dec(&sdata->u.mesh.mshstats.estab_plinks);
	return mesh_accept_plinks_update(sdata);
}

/**
 * mesh_plink_fsm_restart - restart a mesh peer link finite state machine
 *
 * @sta: mesh peer link to restart
 *
 * Locking: this function must be called holding sta->lock
 */
static inline void mesh_plink_fsm_restart(struct sta_info *sta)
{
	sta->plink_state = NL80211_PLINK_LISTEN;
	sta->llid = sta->plid = sta->reason = 0;
	sta->plink_retries = 0;
}

/*
 * Allocate mesh sta entry and insert into station table
 */
static struct sta_info *mesh_plink_alloc(struct ieee80211_sub_if_data *sdata,
					 u8 *hw_addr)
{
	struct sta_info *sta;

	if (sdata->local->num_sta >= MESH_MAX_PLINKS)
		return NULL;

	sta = sta_info_alloc(sdata, hw_addr, GFP_KERNEL);
	if (!sta)
		return NULL;

	sta_info_pre_move_state(sta, IEEE80211_STA_AUTH);
	sta_info_pre_move_state(sta, IEEE80211_STA_ASSOC);
	sta_info_pre_move_state(sta, IEEE80211_STA_AUTHORIZED);

	set_sta_flag(sta, WLAN_STA_WME);

	return sta;
}

/**
 * mesh_set_ht_prot_mode - set correct HT protection mode
 *
 * Section 9.23.3.5 of IEEE 80211-2012 describes the protection rules for HT
 * mesh STA in a MBSS. Three HT protection modes are supported for now, non-HT
 * mixed mode, 20MHz-protection and no-protection mode. non-HT mixed mode is
 * selected if any non-HT peers are present in our MBSS.  20MHz-protection mode
 * is selected if all peers in our 20/40MHz MBSS support HT and atleast one
 * HT20 peer is present. Otherwise no-protection mode is selected.
 */
static u32 mesh_set_ht_prot_mode(struct ieee80211_sub_if_data *sdata)
{
	struct ieee80211_local *local = sdata->local;
	struct sta_info *sta;
	u32 changed = 0;
	u16 ht_opmode;
	bool non_ht_sta = false, ht20_sta = false;

	if (sdata->vif.bss_conf.channel_type == NL80211_CHAN_NO_HT)
		return 0;

	rcu_read_lock();
	list_for_each_entry_rcu(sta, &local->sta_list, list) {
		if (sdata != sta->sdata ||
		    sta->plink_state != NL80211_PLINK_ESTAB)
			continue;

		switch (sta->ch_type) {
		case NL80211_CHAN_NO_HT:
			mpl_dbg(sdata,
				"mesh_plink %pM: nonHT sta (%pM) is present\n",
				sdata->vif.addr, sta->sta.addr);
			non_ht_sta = true;
			goto out;
		case NL80211_CHAN_HT20:
			mpl_dbg(sdata,
				"mesh_plink %pM: HT20 sta (%pM) is present\n",
				sdata->vif.addr, sta->sta.addr);
			ht20_sta = true;
		default:
			break;
		}
	}
out:
	rcu_read_unlock();

	if (non_ht_sta)
		ht_opmode = IEEE80211_HT_OP_MODE_PROTECTION_NONHT_MIXED;
	else if (ht20_sta &&
		 sdata->vif.bss_conf.channel_type > NL80211_CHAN_HT20)
		ht_opmode = IEEE80211_HT_OP_MODE_PROTECTION_20MHZ;
	else
		ht_opmode = IEEE80211_HT_OP_MODE_PROTECTION_NONE;

	if (sdata->vif.bss_conf.ht_operation_mode != ht_opmode) {
		sdata->vif.bss_conf.ht_operation_mode = ht_opmode;
		sdata->u.mesh.mshcfg.ht_opmode = ht_opmode;
		changed = BSS_CHANGED_HT;
		mpl_dbg(sdata,
			"mesh_plink %pM: protection mode changed to %d\n",
			sdata->vif.addr, ht_opmode);
	}

	return changed;
}

/**
 * __mesh_plink_deactivate - deactivate mesh peer link
 *
 * @sta: mesh peer link to deactivate
 *
 * All mesh paths with this peer as next hop will be flushed
 * Returns beacon changed flag if the beacon content changed.
 *
 * Locking: the caller must hold sta->lock
 */
static u32 __mesh_plink_deactivate(struct sta_info *sta)
{
	struct ieee80211_sub_if_data *sdata = sta->sdata;
	u32 changed = 0;

	if (sta->plink_state == NL80211_PLINK_ESTAB)
		changed = mesh_plink_dec_estab_count(sdata);
	sta->plink_state = NL80211_PLINK_BLOCKED;
	mesh_path_flush_by_nexthop(sta);

	return changed;
}

/**
 * mesh_plink_deactivate - deactivate mesh peer link
 *
 * @sta: mesh peer link to deactivate
 *
 * All mesh paths with this peer as next hop will be flushed
 */
void mesh_plink_deactivate(struct sta_info *sta)
{
	struct ieee80211_sub_if_data *sdata = sta->sdata;
	u32 changed;

	spin_lock_bh(&sta->lock);
	changed = __mesh_plink_deactivate(sta);
	sta->reason = cpu_to_le16(WLAN_REASON_MESH_PEER_CANCELED);
	mesh_plink_frame_tx(sdata, WLAN_SP_MESH_PEERING_CLOSE,
			    sta->sta.addr, sta->llid, sta->plid,
			    sta->reason);
	spin_unlock_bh(&sta->lock);

	ieee80211_bss_info_change_notify(sdata, changed);
}

static int mesh_plink_frame_tx(struct ieee80211_sub_if_data *sdata,
		enum ieee80211_self_protected_actioncode action,
		u8 *da, __le16 llid, __le16 plid, __le16 reason) {
	struct ieee80211_local *local = sdata->local;
	struct sk_buff *skb;
	struct ieee80211_tx_info *info;
	struct ieee80211_mgmt *mgmt;
	bool include_plid = false;
	u16 peering_proto = 0;
	u8 *pos, ie_len = 4;
	int hdr_len = offsetof(struct ieee80211_mgmt, u.action.u.self_prot) +
		      sizeof(mgmt->u.action.u.self_prot);
	int err = -ENOMEM;

	skb = dev_alloc_skb(local->tx_headroom +
			    hdr_len +
			    2 + /* capability info */
			    2 + /* AID */
			    2 + 8 + /* supported rates */
			    2 + (IEEE80211_MAX_SUPP_RATES - 8) +
			    2 + sdata->u.mesh.mesh_id_len +
			    2 + sizeof(struct ieee80211_meshconf_ie) +
			    2 + sizeof(struct ieee80211_ht_cap) +
			    2 + sizeof(struct ieee80211_ht_operation) +
			    2 + 8 + /* peering IE */
			    sdata->u.mesh.ie_len);
	if (!skb)
		return -1;
	info = IEEE80211_SKB_CB(skb);
	skb_reserve(skb, local->tx_headroom);
	mgmt = (struct ieee80211_mgmt *) skb_put(skb, hdr_len);
	memset(mgmt, 0, hdr_len);
	mgmt->frame_control = cpu_to_le16(IEEE80211_FTYPE_MGMT |
					  IEEE80211_STYPE_ACTION);
	memcpy(mgmt->da, da, ETH_ALEN);
	memcpy(mgmt->sa, sdata->vif.addr, ETH_ALEN);
	memcpy(mgmt->bssid, sdata->vif.addr, ETH_ALEN);
	mgmt->u.action.category = WLAN_CATEGORY_SELF_PROTECTED;
	mgmt->u.action.u.self_prot.action_code = action;

	if (action != WLAN_SP_MESH_PEERING_CLOSE) {
		/* capability info */
		pos = skb_put(skb, 2);
		memset(pos, 0, 2);
		if (action == WLAN_SP_MESH_PEERING_CONFIRM) {
			/* AID */
			pos = skb_put(skb, 2);
			memcpy(pos + 2, &plid, 2);
		}
		if (ieee80211_add_srates_ie(sdata, skb, true,
					    local->oper_channel->band) ||
		    ieee80211_add_ext_srates_ie(sdata, skb, true,
						local->oper_channel->band) ||
		    mesh_add_rsn_ie(skb, sdata) ||
		    mesh_add_meshid_ie(skb, sdata) ||
		    mesh_add_meshconf_ie(skb, sdata))
			goto free;
	} else {	/* WLAN_SP_MESH_PEERING_CLOSE */
		info->flags |= IEEE80211_TX_CTL_NO_ACK;
		if (mesh_add_meshid_ie(skb, sdata))
			goto free;
	}

	/* Add Mesh Peering Management element */
	switch (action) {
	case WLAN_SP_MESH_PEERING_OPEN:
		break;
	case WLAN_SP_MESH_PEERING_CONFIRM:
		ie_len += 2;
		include_plid = true;
		break;
	case WLAN_SP_MESH_PEERING_CLOSE:
		if (plid) {
			ie_len += 2;
			include_plid = true;
		}
		ie_len += 2;	/* reason code */
		break;
	default:
		err = -EINVAL;
		goto free;
	}

	if (WARN_ON(skb_tailroom(skb) < 2 + ie_len))
		goto free;

	pos = skb_put(skb, 2 + ie_len);
	*pos++ = WLAN_EID_PEER_MGMT;
	*pos++ = ie_len;
	memcpy(pos, &peering_proto, 2);
	pos += 2;
	memcpy(pos, &llid, 2);
	pos += 2;
	if (include_plid) {
		memcpy(pos, &plid, 2);
		pos += 2;
	}
	if (action == WLAN_SP_MESH_PEERING_CLOSE) {
		memcpy(pos, &reason, 2);
		pos += 2;
	}

	if (action != WLAN_SP_MESH_PEERING_CLOSE) {
		if (mesh_add_ht_cap_ie(skb, sdata) ||
		    mesh_add_ht_oper_ie(skb, sdata))
			goto free;
	}

	if (mesh_add_vendor_ies(skb, sdata))
		goto free;

	ieee80211_tx_skb(sdata, skb);
	return 0;
free:
	kfree_skb(skb);
	return err;
}

/**
 * mesh_peer_init - initialize new mesh peer and return resulting sta_info
 *
 * @sdata: local meshif
 * @addr: peer's address
 * @elems: IEs from beacon or mesh peering frame
 *
 * call under RCU
 */
static struct sta_info *mesh_peer_init(struct ieee80211_sub_if_data *sdata,
				       u8 *addr,
				       struct ieee802_11_elems *elems)
{
	struct ieee80211_local *local = sdata->local;
	enum ieee80211_band band = local->oper_channel->band;
	struct ieee80211_supported_band *sband;
	u32 rates, basic_rates = 0;
	struct sta_info *sta;
	bool insert = false;

	sband = local->hw.wiphy->bands[band];
	rates = ieee80211_sta_get_rates(local, elems, band, &basic_rates);

	sta = sta_info_get(sdata, addr);
	if (!sta) {
		/* Userspace handles peer allocation when security is enabled */
		if (sdata->u.mesh.security & IEEE80211_MESH_SEC_AUTHED) {
			cfg80211_notify_new_peer_candidate(sdata->dev, addr,
							   elems->ie_start,
							   elems->total_len,
							   GFP_ATOMIC);
			return NULL;
		}

		sta = mesh_plink_alloc(sdata, addr);
		if (!sta)
			return NULL;
		insert = true;
	}

	spin_lock_bh(&sta->lock);
	sta->last_rx = jiffies;
	if (sta->plink_state == NL80211_PLINK_ESTAB) {
		spin_unlock_bh(&sta->lock);
		return sta;
	}

	sta->sta.supp_rates[band] = rates;
	if (elems->ht_cap_elem &&
	    sdata->vif.bss_conf.channel_type != NL80211_CHAN_NO_HT)
		ieee80211_ht_cap_ie_to_sta_ht_cap(sdata, sband,
						  elems->ht_cap_elem,
						  &sta->sta.ht_cap);
	else
		memset(&sta->sta.ht_cap, 0, sizeof(sta->sta.ht_cap));

	if (elems->ht_operation) {
		if (!(elems->ht_operation->ht_param &
		      IEEE80211_HT_PARAM_CHAN_WIDTH_ANY))
			sta->sta.ht_cap.cap &=
					    ~IEEE80211_HT_CAP_SUP_WIDTH_20_40;
		sta->ch_type =
			ieee80211_ht_oper_to_channel_type(elems->ht_operation);
	}

	rate_control_rate_init(sta);
	spin_unlock_bh(&sta->lock);

	if (insert && sta_info_insert(sta))
		return NULL;

	return sta;
}

void mesh_neighbour_update(struct ieee80211_sub_if_data *sdata,
			   u8 *hw_addr,
			   struct ieee802_11_elems *elems)
{
	struct sta_info *sta;

	rcu_read_lock();
	sta = mesh_peer_init(sdata, hw_addr, elems);
	if (!sta)
		goto out;

	if (mesh_peer_accepts_plinks(elems) &&
	    sta->plink_state == NL80211_PLINK_LISTEN &&
	    sdata->u.mesh.accepting_plinks &&
	    sdata->u.mesh.mshcfg.auto_open_plinks &&
	    rssi_threshold_check(sta, sdata))
		mesh_plink_open(sta);

out:
	rcu_read_unlock();
}

static void mesh_plink_timer(unsigned long data)
{
	struct sta_info *sta;
	__le16 llid, plid, reason;
	struct ieee80211_sub_if_data *sdata;

	/*
	 * This STA is valid because sta_info_destroy() will
	 * del_timer_sync() this timer after having made sure
	 * it cannot be readded (by deleting the plink.)
	 */
	sta = (struct sta_info *) data;

	if (sta->sdata->local->quiescing) {
		sta->plink_timer_was_running = true;
		return;
	}

	spin_lock_bh(&sta->lock);
	if (sta->ignore_plink_timer) {
		sta->ignore_plink_timer = false;
		spin_unlock_bh(&sta->lock);
		return;
	}
	mpl_dbg(sta->sdata,
		"Mesh plink timer for %pM fired on state %d\n",
		sta->sta.addr, sta->plink_state);
	reason = 0;
	llid = sta->llid;
	plid = sta->plid;
	sdata = sta->sdata;

	switch (sta->plink_state) {
	case NL80211_PLINK_OPN_RCVD:
	case NL80211_PLINK_OPN_SNT:
		/* retry timer */
		if (sta->plink_retries < dot11MeshMaxRetries(sdata)) {
			u32 rand;
			mpl_dbg(sta->sdata,
				"Mesh plink for %pM (retry, timeout): %d %d\n",
				sta->sta.addr, sta->plink_retries,
				sta->plink_timeout);
			get_random_bytes(&rand, sizeof(u32));
			sta->plink_timeout = sta->plink_timeout +
					     rand % sta->plink_timeout;
			++sta->plink_retries;
			mod_plink_timer(sta, sta->plink_timeout);
			spin_unlock_bh(&sta->lock);
			mesh_plink_frame_tx(sdata, WLAN_SP_MESH_PEERING_OPEN,
					    sta->sta.addr, llid, 0, 0);
			break;
		}
		reason = cpu_to_le16(WLAN_REASON_MESH_MAX_RETRIES);
		/* fall through on else */
	case NL80211_PLINK_CNF_RCVD:
		/* confirm timer */
		if (!reason)
			reason = cpu_to_le16(WLAN_REASON_MESH_CONFIRM_TIMEOUT);
		sta->plink_state = NL80211_PLINK_HOLDING;
		mod_plink_timer(sta, dot11MeshHoldingTimeout(sdata));
		spin_unlock_bh(&sta->lock);
		mesh_plink_frame_tx(sdata, WLAN_SP_MESH_PEERING_CLOSE,
				    sta->sta.addr, llid, plid, reason);
		break;
	case NL80211_PLINK_HOLDING:
		/* holding timer */
		del_timer(&sta->plink_timer);
		mesh_plink_fsm_restart(sta);
		spin_unlock_bh(&sta->lock);
		break;
	default:
		spin_unlock_bh(&sta->lock);
		break;
	}
}

#ifdef CONFIG_PM
void mesh_plink_quiesce(struct sta_info *sta)
{
	if (del_timer_sync(&sta->plink_timer))
		sta->plink_timer_was_running = true;
}

void mesh_plink_restart(struct sta_info *sta)
{
	if (sta->plink_timer_was_running) {
		add_timer(&sta->plink_timer);
		sta->plink_timer_was_running = false;
	}
}
#endif

static inline void mesh_plink_timer_set(struct sta_info *sta, int timeout)
{
	sta->plink_timer.expires = jiffies + (HZ * timeout / 1000);
	sta->plink_timer.data = (unsigned long) sta;
	sta->plink_timer.function = mesh_plink_timer;
	sta->plink_timeout = timeout;
	add_timer(&sta->plink_timer);
}

int mesh_plink_open(struct sta_info *sta)
{
	__le16 llid;
	struct ieee80211_sub_if_data *sdata = sta->sdata;

	if (!test_sta_flag(sta, WLAN_STA_AUTH))
		return -EPERM;

	spin_lock_bh(&sta->lock);
	get_random_bytes(&llid, 2);
	sta->llid = llid;
	if (sta->plink_state != NL80211_PLINK_LISTEN &&
	    sta->plink_state != NL80211_PLINK_BLOCKED) {
		spin_unlock_bh(&sta->lock);
		return -EBUSY;
	}
	sta->plink_state = NL80211_PLINK_OPN_SNT;
	mesh_plink_timer_set(sta, dot11MeshRetryTimeout(sdata));
	spin_unlock_bh(&sta->lock);
	mpl_dbg(sdata,
		"Mesh plink: starting establishment with %pM\n",
		sta->sta.addr);

	return mesh_plink_frame_tx(sdata, WLAN_SP_MESH_PEERING_OPEN,
				   sta->sta.addr, llid, 0, 0);
}

void mesh_plink_block(struct sta_info *sta)
{
	struct ieee80211_sub_if_data *sdata = sta->sdata;
	u32 changed;

	spin_lock_bh(&sta->lock);
	changed = __mesh_plink_deactivate(sta);
	sta->plink_state = NL80211_PLINK_BLOCKED;
	spin_unlock_bh(&sta->lock);

	ieee80211_bss_info_change_notify(sdata, changed);
}


void mesh_rx_plink_frame(struct ieee80211_sub_if_data *sdata, struct ieee80211_mgmt *mgmt,
			 size_t len, struct ieee80211_rx_status *rx_status)
{
	struct ieee802_11_elems elems;
	struct sta_info *sta;
	enum plink_event event;
	enum ieee80211_self_protected_actioncode ftype;
	size_t baselen;
	bool matches_local = true;
	u8 ie_len;
	u8 *baseaddr;
	u32 changed = 0;
	__le16 plid, llid, reason;
	static const char *mplstates[] = {
		[NL80211_PLINK_LISTEN] = "LISTEN",
		[NL80211_PLINK_OPN_SNT] = "OPN-SNT",
		[NL80211_PLINK_OPN_RCVD] = "OPN-RCVD",
		[NL80211_PLINK_CNF_RCVD] = "CNF_RCVD",
		[NL80211_PLINK_ESTAB] = "ESTAB",
		[NL80211_PLINK_HOLDING] = "HOLDING",
		[NL80211_PLINK_BLOCKED] = "BLOCKED"
	};

	/* need action_code, aux */
	if (len < IEEE80211_MIN_ACTION_SIZE + 3)
		return;

	if (is_multicast_ether_addr(mgmt->da)) {
		mpl_dbg(sdata,
			"Mesh plink: ignore frame from multicast address\n");
		return;
	}

	baseaddr = mgmt->u.action.u.self_prot.variable;
	baselen = (u8 *) mgmt->u.action.u.self_prot.variable - (u8 *) mgmt;
	if (mgmt->u.action.u.self_prot.action_code ==
						WLAN_SP_MESH_PEERING_CONFIRM) {
		baseaddr += 4;
		baselen += 4;
	}
	ieee802_11_parse_elems(baseaddr, len - baselen, &elems);
	if (!elems.peering) {
		mpl_dbg(sdata,
			"Mesh plink: missing necessary peer link ie\n");
		return;
	}
	if (elems.rsn_len &&
			sdata->u.mesh.security == IEEE80211_MESH_SEC_NONE) {
		mpl_dbg(sdata,
			"Mesh plink: can't establish link with secure peer\n");
		return;
	}

	ftype = mgmt->u.action.u.self_prot.action_code;
	ie_len = elems.peering_len;
	if ((ftype == WLAN_SP_MESH_PEERING_OPEN && ie_len != 4) ||
	    (ftype == WLAN_SP_MESH_PEERING_CONFIRM && ie_len != 6) ||
	    (ftype == WLAN_SP_MESH_PEERING_CLOSE && ie_len != 6
							&& ie_len != 8)) {
		mpl_dbg(sdata,
			"Mesh plink: incorrect plink ie length %d %d\n",
			ftype, ie_len);
		return;
	}

	if (ftype != WLAN_SP_MESH_PEERING_CLOSE &&
				(!elems.mesh_id || !elems.mesh_config)) {
		mpl_dbg(sdata, "Mesh plink: missing necessary ie\n");
		return;
	}
	/* Note the lines below are correct, the llid in the frame is the plid
	 * from the point of view of this host.
	 */
	memcpy(&plid, PLINK_GET_LLID(elems.peering), 2);
	if (ftype == WLAN_SP_MESH_PEERING_CONFIRM ||
	    (ftype == WLAN_SP_MESH_PEERING_CLOSE && ie_len == 8))
		memcpy(&llid, PLINK_GET_PLID(elems.peering), 2);

	rcu_read_lock();

	sta = sta_info_get(sdata, mgmt->sa);
	if (!sta && ftype != WLAN_SP_MESH_PEERING_OPEN) {
		mpl_dbg(sdata, "Mesh plink: cls or cnf from unknown peer\n");
		rcu_read_unlock();
		return;
	}

	if (ftype == WLAN_SP_MESH_PEERING_OPEN &&
	    !rssi_threshold_check(sta, sdata)) {
		mpl_dbg(sdata, "Mesh plink: %pM does not meet rssi threshold\n",
			mgmt->sa);
		rcu_read_unlock();
		return;
	}

	if (sta && !test_sta_flag(sta, WLAN_STA_AUTH)) {
		mpl_dbg(sdata, "Mesh plink: Action frame from non-authed peer\n");
		rcu_read_unlock();
		return;
	}

	if (sta && sta->plink_state == NL80211_PLINK_BLOCKED) {
		rcu_read_unlock();
		return;
	}

	/* Now we will figure out the appropriate event... */
	event = PLINK_UNDEFINED;
	if (ftype != WLAN_SP_MESH_PEERING_CLOSE &&
	    !mesh_matches_local(sdata, &elems)) {
		matches_local = false;
		switch (ftype) {
		case WLAN_SP_MESH_PEERING_OPEN:
			event = OPN_RJCT;
			break;
		case WLAN_SP_MESH_PEERING_CONFIRM:
			event = CNF_RJCT;
			break;
		default:
			break;
		}
	}

	if (!sta && !matches_local) {
		rcu_read_unlock();
		reason = cpu_to_le16(WLAN_REASON_MESH_CONFIG);
		llid = 0;
		mesh_plink_frame_tx(sdata, WLAN_SP_MESH_PEERING_CLOSE,
				    mgmt->sa, llid, plid, reason);
		return;
	} else if (!sta) {
		/* ftype == WLAN_SP_MESH_PEERING_OPEN */
		if (!mesh_plink_free_count(sdata)) {
			mpl_dbg(sdata, "Mesh plink error: no more free plinks\n");
			rcu_read_unlock();
			return;
		}
		event = OPN_ACPT;
	} else if (matches_local) {
		switch (ftype) {
		case WLAN_SP_MESH_PEERING_OPEN:
			if (!mesh_plink_free_count(sdata) ||
			    (sta->plid && sta->plid != plid))
				event = OPN_IGNR;
			else
				event = OPN_ACPT;
			break;
		case WLAN_SP_MESH_PEERING_CONFIRM:
			if (!mesh_plink_free_count(sdata) ||
			    (sta->llid != llid || sta->plid != plid))
				event = CNF_IGNR;
			else
				event = CNF_ACPT;
			break;
		case WLAN_SP_MESH_PEERING_CLOSE:
			if (sta->plink_state == NL80211_PLINK_ESTAB)
				/* Do not check for llid or plid. This does not
				 * follow the standard but since multiple plinks
				 * per sta are not supported, it is necessary in
				 * order to avoid a livelock when MP A sees an
				 * establish peer link to MP B but MP B does not
				 * see it. This can be caused by a timeout in
				 * B's peer link establishment or B beign
				 * restarted.
				 */
				event = CLS_ACPT;
			else if (sta->plid != plid)
				event = CLS_IGNR;
			else if (ie_len == 7 && sta->llid != llid)
				event = CLS_IGNR;
			else
				event = CLS_ACPT;
			break;
		default:
			mpl_dbg(sdata, "Mesh plink: unknown frame subtype\n");
			rcu_read_unlock();
			return;
		}
	}

	if (event == OPN_ACPT) {
		/* allocate sta entry if necessary and update info */
		sta = mesh_peer_init(sdata, mgmt->sa, &elems);
		if (!sta) {
			mpl_dbg(sdata, "Mesh plink: failed to init peer!\n");
			rcu_read_unlock();
			return;
		}
	}

	mpl_dbg(sdata,
		"Mesh plink (peer, state, llid, plid, event): %pM %s %d %d %d\n",
		mgmt->sa, mplstates[sta->plink_state],
		le16_to_cpu(sta->llid), le16_to_cpu(sta->plid),
		event);
	reason = 0;
	spin_lock_bh(&sta->lock);
	switch (sta->plink_state) {
		/* spin_unlock as soon as state is updated at each case */
	case NL80211_PLINK_LISTEN:
		switch (event) {
		case CLS_ACPT:
			mesh_plink_fsm_restart(sta);
			spin_unlock_bh(&sta->lock);
			break;
		case OPN_ACPT:
			sta->plink_state = NL80211_PLINK_OPN_RCVD;
			sta->plid = plid;
			get_random_bytes(&llid, 2);
			sta->llid = llid;
			mesh_plink_timer_set(sta, dot11MeshRetryTimeout(sdata));
			spin_unlock_bh(&sta->lock);
			mesh_plink_frame_tx(sdata,
					    WLAN_SP_MESH_PEERING_OPEN,
					    sta->sta.addr, llid, 0, 0);
			mesh_plink_frame_tx(sdata,
					    WLAN_SP_MESH_PEERING_CONFIRM,
					    sta->sta.addr, llid, plid, 0);
			break;
		default:
			spin_unlock_bh(&sta->lock);
			break;
		}
		break;

	case NL80211_PLINK_OPN_SNT:
		switch (event) {
		case OPN_RJCT:
		case CNF_RJCT:
			reason = cpu_to_le16(WLAN_REASON_MESH_CONFIG);
		case CLS_ACPT:
			if (!reason)
				reason = cpu_to_le16(WLAN_REASON_MESH_CLOSE);
			sta->reason = reason;
			sta->plink_state = NL80211_PLINK_HOLDING;
			if (!mod_plink_timer(sta,
					     dot11MeshHoldingTimeout(sdata)))
				sta->ignore_plink_timer = true;

			llid = sta->llid;
			spin_unlock_bh(&sta->lock);
			mesh_plink_frame_tx(sdata,
					    WLAN_SP_MESH_PEERING_CLOSE,
					    sta->sta.addr, llid, plid, reason);
			break;
		case OPN_ACPT:
			/* retry timer is left untouched */
			sta->plink_state = NL80211_PLINK_OPN_RCVD;
			sta->plid = plid;
			llid = sta->llid;
			spin_unlock_bh(&sta->lock);
			mesh_plink_frame_tx(sdata,
					    WLAN_SP_MESH_PEERING_CONFIRM,
					    sta->sta.addr, llid, plid, 0);
			break;
		case CNF_ACPT:
			sta->plink_state = NL80211_PLINK_CNF_RCVD;
			if (!mod_plink_timer(sta,
					     dot11MeshConfirmTimeout(sdata)))
				sta->ignore_plink_timer = true;

			spin_unlock_bh(&sta->lock);
			break;
		default:
			spin_unlock_bh(&sta->lock);
			break;
		}
		break;

	case NL80211_PLINK_OPN_RCVD:
		switch (event) {
		case OPN_RJCT:
		case CNF_RJCT:
			reason = cpu_to_le16(WLAN_REASON_MESH_CONFIG);
		case CLS_ACPT:
			if (!reason)
				reason = cpu_to_le16(WLAN_REASON_MESH_CLOSE);
			sta->reason = reason;
			sta->plink_state = NL80211_PLINK_HOLDING;
			if (!mod_plink_timer(sta,
					     dot11MeshHoldingTimeout(sdata)))
				sta->ignore_plink_timer = true;

			llid = sta->llid;
			spin_unlock_bh(&sta->lock);
			mesh_plink_frame_tx(sdata, WLAN_SP_MESH_PEERING_CLOSE,
					    sta->sta.addr, llid, plid, reason);
			break;
		case OPN_ACPT:
			llid = sta->llid;
			spin_unlock_bh(&sta->lock);
			mesh_plink_frame_tx(sdata,
					    WLAN_SP_MESH_PEERING_CONFIRM,
					    sta->sta.addr, llid, plid, 0);
			break;
		case CNF_ACPT:
			del_timer(&sta->plink_timer);
			sta->plink_state = NL80211_PLINK_ESTAB;
			spin_unlock_bh(&sta->lock);
			changed |= mesh_plink_inc_estab_count(sdata);
			changed |= mesh_set_ht_prot_mode(sdata);
			mpl_dbg(sdata, "Mesh plink with %pM ESTABLISHED\n",
				sta->sta.addr);
			break;
		default:
			spin_unlock_bh(&sta->lock);
			break;
		}
		break;

	case NL80211_PLINK_CNF_RCVD:
		switch (event) {
		case OPN_RJCT:
		case CNF_RJCT:
			reason = cpu_to_le16(WLAN_REASON_MESH_CONFIG);
		case CLS_ACPT:
			if (!reason)
				reason = cpu_to_le16(WLAN_REASON_MESH_CLOSE);
			sta->reason = reason;
			sta->plink_state = NL80211_PLINK_HOLDING;
			if (!mod_plink_timer(sta,
					     dot11MeshHoldingTimeout(sdata)))
				sta->ignore_plink_timer = true;

			llid = sta->llid;
			spin_unlock_bh(&sta->lock);
			mesh_plink_frame_tx(sdata,
					    WLAN_SP_MESH_PEERING_CLOSE,
					    sta->sta.addr, llid, plid, reason);
			break;
		case OPN_ACPT:
			del_timer(&sta->plink_timer);
			sta->plink_state = NL80211_PLINK_ESTAB;
			spin_unlock_bh(&sta->lock);
			changed |= mesh_plink_inc_estab_count(sdata);
			changed |= mesh_set_ht_prot_mode(sdata);
			mpl_dbg(sdata, "Mesh plink with %pM ESTABLISHED\n",
				sta->sta.addr);
			mesh_plink_frame_tx(sdata,
					    WLAN_SP_MESH_PEERING_CONFIRM,
					    sta->sta.addr, llid, plid, 0);
			break;
		default:
			spin_unlock_bh(&sta->lock);
			break;
		}
		break;

	case NL80211_PLINK_ESTAB:
		switch (event) {
		case CLS_ACPT:
			reason = cpu_to_le16(WLAN_REASON_MESH_CLOSE);
			sta->reason = reason;
			changed |= __mesh_plink_deactivate(sta);
			sta->plink_state = NL80211_PLINK_HOLDING;
			llid = sta->llid;
			mod_plink_timer(sta, dot11MeshHoldingTimeout(sdata));
			spin_unlock_bh(&sta->lock);
			changed |= mesh_set_ht_prot_mode(sdata);
			mesh_plink_frame_tx(sdata, WLAN_SP_MESH_PEERING_CLOSE,
					    sta->sta.addr, llid, plid, reason);
			break;
		case OPN_ACPT:
			llid = sta->llid;
			spin_unlock_bh(&sta->lock);
			mesh_plink_frame_tx(sdata,
					    WLAN_SP_MESH_PEERING_CONFIRM,
					    sta->sta.addr, llid, plid, 0);
			break;
		default:
			spin_unlock_bh(&sta->lock);
			break;
		}
		break;
	case NL80211_PLINK_HOLDING:
		switch (event) {
		case CLS_ACPT:
			if (del_timer(&sta->plink_timer))
				sta->ignore_plink_timer = 1;
			mesh_plink_fsm_restart(sta);
			spin_unlock_bh(&sta->lock);
			break;
		case OPN_ACPT:
		case CNF_ACPT:
		case OPN_RJCT:
		case CNF_RJCT:
			llid = sta->llid;
			reason = sta->reason;
			spin_unlock_bh(&sta->lock);
			mesh_plink_frame_tx(sdata, WLAN_SP_MESH_PEERING_CLOSE,
					    sta->sta.addr, llid, plid, reason);
			break;
		default:
			spin_unlock_bh(&sta->lock);
		}
		break;
	default:
		/* should not get here, PLINK_BLOCKED is dealt with at the
		 * beginning of the function
		 */
		spin_unlock_bh(&sta->lock);
		break;
	}

	rcu_read_unlock();

	if (changed)
		ieee80211_bss_info_change_notify(sdata, changed);
}
