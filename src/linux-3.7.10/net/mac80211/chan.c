/*
 * mac80211 - channel management
 */

#include <linux/nl80211.h>
#include <net/cfg80211.h>
#include "ieee80211_i.h"

static enum ieee80211_chan_mode
__ieee80211_get_channel_mode(struct ieee80211_local *local,
			     struct ieee80211_sub_if_data *ignore)
{
	struct ieee80211_sub_if_data *sdata;

	lockdep_assert_held(&local->iflist_mtx);

	list_for_each_entry(sdata, &local->interfaces, list) {
		if (sdata == ignore)
			continue;

		if (!ieee80211_sdata_running(sdata))
			continue;

		switch (sdata->vif.type) {
		case NL80211_IFTYPE_MONITOR:
			continue;
		case NL80211_IFTYPE_STATION:
			if (!sdata->u.mgd.associated)
				continue;
			break;
		case NL80211_IFTYPE_ADHOC:
			if (!sdata->u.ibss.ssid_len)
				continue;
			if (!sdata->u.ibss.fixed_channel)
				return CHAN_MODE_HOPPING;
			break;
		case NL80211_IFTYPE_AP_VLAN:
			/* will also have _AP interface */
			continue;
		case NL80211_IFTYPE_AP:
			if (!sdata->u.ap.beacon)
				continue;
			break;
		case NL80211_IFTYPE_MESH_POINT:
			if (!sdata->wdev.mesh_id_len)
				continue;
			break;
		default:
			break;
		}

		return CHAN_MODE_FIXED;
	}

	return CHAN_MODE_UNDEFINED;
}

enum ieee80211_chan_mode
ieee80211_get_channel_mode(struct ieee80211_local *local,
			   struct ieee80211_sub_if_data *ignore)
{
	enum ieee80211_chan_mode mode;

	mutex_lock(&local->iflist_mtx);
	mode = __ieee80211_get_channel_mode(local, ignore);
	mutex_unlock(&local->iflist_mtx);

	return mode;
}

static enum nl80211_channel_type
ieee80211_get_superchan(struct ieee80211_local *local,
			struct ieee80211_sub_if_data *sdata)
{
	enum nl80211_channel_type superchan = NL80211_CHAN_NO_HT;
	struct ieee80211_sub_if_data *tmp;

	mutex_lock(&local->iflist_mtx);
	list_for_each_entry(tmp, &local->interfaces, list) {
		if (tmp == sdata)
			continue;

		if (!ieee80211_sdata_running(tmp))
			continue;

		switch (tmp->vif.bss_conf.channel_type) {
		case NL80211_CHAN_NO_HT:
		case NL80211_CHAN_HT20:
			if (superchan > tmp->vif.bss_conf.channel_type)
				break;

			superchan = tmp->vif.bss_conf.channel_type;
			break;
		case NL80211_CHAN_HT40PLUS:
			WARN_ON(superchan == NL80211_CHAN_HT40MINUS);
			superchan = NL80211_CHAN_HT40PLUS;
			break;
		case NL80211_CHAN_HT40MINUS:
			WARN_ON(superchan == NL80211_CHAN_HT40PLUS);
			superchan = NL80211_CHAN_HT40MINUS;
			break;
		}
	}
	mutex_unlock(&local->iflist_mtx);

	return superchan;
}

static bool
ieee80211_channel_types_are_compatible(enum nl80211_channel_type chantype1,
				       enum nl80211_channel_type chantype2,
				       enum nl80211_channel_type *compat)
{
	/*
	 * start out with chantype1 being the result,
	 * overwriting later if needed
	 */
	if (compat)
		*compat = chantype1;

	switch (chantype1) {
	case NL80211_CHAN_NO_HT:
		if (compat)
			*compat = chantype2;
		break;
	case NL80211_CHAN_HT20:
		/*
		 * allow any change that doesn't go to no-HT
		 * (if it already is no-HT no change is needed)
		 */
		if (chantype2 == NL80211_CHAN_NO_HT)
			break;
		if (compat)
			*compat = chantype2;
		break;
	case NL80211_CHAN_HT40PLUS:
	case NL80211_CHAN_HT40MINUS:
		/* allow smaller bandwidth and same */
		if (chantype2 == NL80211_CHAN_NO_HT)
			break;
		if (chantype2 == NL80211_CHAN_HT20)
			break;
		if (chantype2 == chantype1)
			break;
		return false;
	}

	return true;
}

bool ieee80211_set_channel_type(struct ieee80211_local *local,
				struct ieee80211_sub_if_data *sdata,
				enum nl80211_channel_type chantype)
{
	enum nl80211_channel_type superchan;
	enum nl80211_channel_type compatchan;

	superchan = ieee80211_get_superchan(local, sdata);
	if (!ieee80211_channel_types_are_compatible(superchan, chantype,
						    &compatchan))
		return false;

	local->_oper_channel_type = compatchan;

	if (sdata)
		sdata->vif.bss_conf.channel_type = chantype;

	return true;

}
