#include <linux/ieee80211.h>
#include <linux/export.h>
#include <net/cfg80211.h>
#include "nl80211.h"
#include "core.h"

/* Default values, timeouts in ms */
#define MESH_TTL 		31
#define MESH_DEFAULT_ELEMENT_TTL 31
#define MESH_MAX_RETR	 	3
#define MESH_RET_T 		100
#define MESH_CONF_T 		100
#define MESH_HOLD_T 		100

#define MESH_PATH_TIMEOUT	5000
#define MESH_RANN_INTERVAL      5000
#define MESH_PATH_TO_ROOT_TIMEOUT      6000
#define MESH_ROOT_INTERVAL     5000
#define MESH_ROOT_CONFIRMATION_INTERVAL 2000

/*
 * Minimum interval between two consecutive PREQs originated by the same
 * interface
 */
#define MESH_PREQ_MIN_INT	10
#define MESH_PERR_MIN_INT	100
#define MESH_DIAM_TRAVERSAL_TIME 50

#define MESH_RSSI_THRESHOLD	0

/*
 * A path will be refreshed if it is used PATH_REFRESH_TIME milliseconds
 * before timing out.  This way it will remain ACTIVE and no data frames
 * will be unnecessarily held in the pending queue.
 */
#define MESH_PATH_REFRESH_TIME			1000
#define MESH_MIN_DISCOVERY_TIMEOUT (2 * MESH_DIAM_TRAVERSAL_TIME)

/* Default maximum number of established plinks per interface */
#define MESH_MAX_ESTAB_PLINKS	32

#define MESH_MAX_PREQ_RETRIES	4

#define MESH_SYNC_NEIGHBOR_OFFSET_MAX 50

const struct mesh_config default_mesh_config = {
	.dot11MeshRetryTimeout = MESH_RET_T,
	.dot11MeshConfirmTimeout = MESH_CONF_T,
	.dot11MeshHoldingTimeout = MESH_HOLD_T,
	.dot11MeshMaxRetries = MESH_MAX_RETR,
	.dot11MeshTTL = MESH_TTL,
	.element_ttl = MESH_DEFAULT_ELEMENT_TTL,
	.auto_open_plinks = true,
	.dot11MeshMaxPeerLinks = MESH_MAX_ESTAB_PLINKS,
	.dot11MeshNbrOffsetMaxNeighbor = MESH_SYNC_NEIGHBOR_OFFSET_MAX,
	.dot11MeshHWMPactivePathTimeout = MESH_PATH_TIMEOUT,
	.dot11MeshHWMPpreqMinInterval = MESH_PREQ_MIN_INT,
	.dot11MeshHWMPperrMinInterval = MESH_PERR_MIN_INT,
	.dot11MeshHWMPnetDiameterTraversalTime = MESH_DIAM_TRAVERSAL_TIME,
	.dot11MeshHWMPmaxPREQretries = MESH_MAX_PREQ_RETRIES,
	.path_refresh_time = MESH_PATH_REFRESH_TIME,
	.min_discovery_timeout = MESH_MIN_DISCOVERY_TIMEOUT,
	.dot11MeshHWMPRannInterval = MESH_RANN_INTERVAL,
	.dot11MeshGateAnnouncementProtocol = false,
	.dot11MeshForwarding = true,
	.rssi_threshold = MESH_RSSI_THRESHOLD,
	.ht_opmode = IEEE80211_HT_OP_MODE_PROTECTION_NONHT_MIXED,
	.dot11MeshHWMPactivePathToRootTimeout = MESH_PATH_TO_ROOT_TIMEOUT,
	.dot11MeshHWMProotInterval = MESH_ROOT_INTERVAL,
	.dot11MeshHWMPconfirmationInterval = MESH_ROOT_CONFIRMATION_INTERVAL,
};

const struct mesh_setup default_mesh_setup = {
	/* cfg80211_join_mesh() will pick a channel if needed */
	.channel = NULL,
	.channel_type = NL80211_CHAN_NO_HT,
	.sync_method = IEEE80211_SYNC_METHOD_NEIGHBOR_OFFSET,
	.path_sel_proto = IEEE80211_PATH_PROTOCOL_HWMP,
	.path_metric = IEEE80211_PATH_METRIC_AIRTIME,
	.ie = NULL,
	.ie_len = 0,
	.is_secure = false,
};

int __cfg80211_join_mesh(struct cfg80211_registered_device *rdev,
			 struct net_device *dev,
			 struct mesh_setup *setup,
			 const struct mesh_config *conf)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	int err;

	BUILD_BUG_ON(IEEE80211_MAX_SSID_LEN != IEEE80211_MAX_MESH_ID_LEN);

	ASSERT_WDEV_LOCK(wdev);

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_MESH_POINT)
		return -EOPNOTSUPP;

	if (!(rdev->wiphy.flags & WIPHY_FLAG_MESH_AUTH) &&
	      setup->is_secure)
		return -EOPNOTSUPP;

	if (wdev->mesh_id_len)
		return -EALREADY;

	if (!setup->mesh_id_len)
		return -EINVAL;

	if (!rdev->ops->join_mesh)
		return -EOPNOTSUPP;

	if (!setup->channel) {
		/* if no channel explicitly given, use preset channel */
		setup->channel = wdev->preset_chan;
		setup->channel_type = wdev->preset_chantype;
	}

	if (!setup->channel) {
		/* if we don't have that either, use the first usable channel */
		enum ieee80211_band band;

		for (band = 0; band < IEEE80211_NUM_BANDS; band++) {
			struct ieee80211_supported_band *sband;
			struct ieee80211_channel *chan;
			int i;

			sband = rdev->wiphy.bands[band];
			if (!sband)
				continue;

			for (i = 0; i < sband->n_channels; i++) {
				chan = &sband->channels[i];
				if (chan->flags & (IEEE80211_CHAN_NO_IBSS |
						   IEEE80211_CHAN_PASSIVE_SCAN |
						   IEEE80211_CHAN_DISABLED |
						   IEEE80211_CHAN_RADAR))
					continue;
				setup->channel = chan;
				break;
			}

			if (setup->channel)
				break;
		}

		/* no usable channel ... */
		if (!setup->channel)
			return -EINVAL;

		setup->channel_type = NL80211_CHAN_NO_HT;
	}

	if (!cfg80211_can_beacon_sec_chan(&rdev->wiphy, setup->channel,
					  setup->channel_type))
		return -EINVAL;

	err = cfg80211_can_use_chan(rdev, wdev, setup->channel,
				    CHAN_MODE_SHARED);
	if (err)
		return err;

	err = rdev->ops->join_mesh(&rdev->wiphy, dev, conf, setup);
	if (!err) {
		memcpy(wdev->ssid, setup->mesh_id, setup->mesh_id_len);
		wdev->mesh_id_len = setup->mesh_id_len;
		wdev->channel = setup->channel;
	}

	return err;
}

int cfg80211_join_mesh(struct cfg80211_registered_device *rdev,
		       struct net_device *dev,
		       struct mesh_setup *setup,
		       const struct mesh_config *conf)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	int err;

	mutex_lock(&rdev->devlist_mtx);
	wdev_lock(wdev);
	err = __cfg80211_join_mesh(rdev, dev, setup, conf);
	wdev_unlock(wdev);
	mutex_unlock(&rdev->devlist_mtx);

	return err;
}

int cfg80211_set_mesh_freq(struct cfg80211_registered_device *rdev,
			   struct wireless_dev *wdev, int freq,
			   enum nl80211_channel_type channel_type)
{
	struct ieee80211_channel *channel;
	int err;

	channel = rdev_freq_to_chan(rdev, freq, channel_type);
	if (!channel || !cfg80211_can_beacon_sec_chan(&rdev->wiphy,
						      channel,
						      channel_type)) {
		return -EINVAL;
	}

	/*
	 * Workaround for libertas (only!), it puts the interface
	 * into mesh mode but doesn't implement join_mesh. Instead,
	 * it is configured via sysfs and then joins the mesh when
	 * you set the channel. Note that the libertas mesh isn't
	 * compatible with 802.11 mesh.
	 */
	if (rdev->ops->libertas_set_mesh_channel) {
		if (channel_type != NL80211_CHAN_NO_HT)
			return -EINVAL;

		if (!netif_running(wdev->netdev))
			return -ENETDOWN;

		err = cfg80211_can_use_chan(rdev, wdev, channel,
					    CHAN_MODE_SHARED);
		if (err)
			return err;

		err = rdev->ops->libertas_set_mesh_channel(&rdev->wiphy,
							   wdev->netdev,
							   channel);
		if (!err)
			wdev->channel = channel;

		return err;
	}

	if (wdev->mesh_id_len)
		return -EBUSY;

	wdev->preset_chan = channel;
	wdev->preset_chantype = channel_type;
	return 0;
}

void cfg80211_notify_new_peer_candidate(struct net_device *dev,
		const u8 *macaddr, const u8* ie, u8 ie_len, gfp_t gfp)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;

	if (WARN_ON(wdev->iftype != NL80211_IFTYPE_MESH_POINT))
		return;

	nl80211_send_new_peer_candidate(wiphy_to_dev(wdev->wiphy), dev,
			macaddr, ie, ie_len, gfp);
}
EXPORT_SYMBOL(cfg80211_notify_new_peer_candidate);

static int __cfg80211_leave_mesh(struct cfg80211_registered_device *rdev,
				 struct net_device *dev)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	int err;

	ASSERT_WDEV_LOCK(wdev);

	if (dev->ieee80211_ptr->iftype != NL80211_IFTYPE_MESH_POINT)
		return -EOPNOTSUPP;

	if (!rdev->ops->leave_mesh)
		return -EOPNOTSUPP;

	if (!wdev->mesh_id_len)
		return -ENOTCONN;

	err = rdev->ops->leave_mesh(&rdev->wiphy, dev);
	if (!err) {
		wdev->mesh_id_len = 0;
		wdev->channel = NULL;
	}

	return err;
}

int cfg80211_leave_mesh(struct cfg80211_registered_device *rdev,
			struct net_device *dev)
{
	struct wireless_dev *wdev = dev->ieee80211_ptr;
	int err;

	wdev_lock(wdev);
	err = __cfg80211_leave_mesh(rdev, dev);
	wdev_unlock(wdev);

	return err;
}
