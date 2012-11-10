#ifndef CLIENT_P_H
#define CLIENT_P_H

#include "antigate.h"

QT_FORWARD_DECLARE_CLASS(QNetworkAccessManager)

namespace Antigate {

class Q_DECL_HIDDEN ClientPrivate {
	Q_DECLARE_PUBLIC(Client)
	Client* q_ptr;
public:
	void setNetworkAccessManager(QNetworkAccessManager* mgr);
	qulonglong submitCaptcha(const QByteArray& key, const QByteArray& captcha, Antigate::CaptchaFormat format, bool phrase, bool regsense, int numeric, bool calc, int minlen, int maxlen, int isrussian);
	void checkCaptchaStatus(const QByteArray& key, qulonglong id);
	void reportCaptcha(const QByteArray& key, qulonglong id);
	void queryBalance(const QByteArray& key);
	void querySystemLoad(void);
	void checkLoginExists(const QByteArray& tkey, const QByteArray& login);
	void transferFunds(const QByteArray& tkey, const QByteArray& login, double amount);
	void getProxies(const QByteArray& key, Antigate::ProxyType type);

private:
	QNetworkAccessManager* m_mgr;

	explicit ClientPrivate(Client* p);

	QNetworkAccessManager* mgr(void) const;

	void _q_captchaSubmitted(void);
	void _q_captchaQueried(void);
	void _q_captchaReported(void);
	void _q_balanceReady(void);
	void _q_sysLoadReady(void);
	void _q_loginCheckReady(void);
	void _q_transferReady(void);
	void _q_proxiesReady(void);

	static Antigate::Error string2error(const QByteArray& code);
};

}

#endif // CLIENT_P_H
