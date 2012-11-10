#ifndef CLIENT_H
#define CLIENT_H

#include "antigate.h"
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

namespace Antigate {

class ClientPrivate;

class ANTIGATE_EXPORT Client : public QObject {
	Q_OBJECT
public:
	explicit Client(QObject* parent = 0);
	virtual ~Client(void);

	qulonglong submitCaptcha(const QByteArray& key, const QByteArray& captcha, Antigate::CaptchaFormat format, bool phrase = false, bool regsense = false, int numeric = 0, bool calc = false, int minlen = 0, int maxlen = 0, int isrussian = 0);
	void checkCaptchaStatus(const QByteArray& key, qulonglong id);
	void reportCaptcha(const QByteArray& key, qulonglong id);
	void queryBalance(const QByteArray& key);
	void querySystemLoad(void);
	void checkLoginExists(const QByteArray& tkey, const QByteArray& login);
	void transferFunds(const QByteArray& tkey, const QByteArray& login, double amount);
	void getProxies(const QByteArray& key, Antigate::ProxyType type);

Q_SIGNALS:
	void captchaSubmitted(Antigate::Error e, qulonglong request_id, qulonglong id);
	void captchaStatus(Antigate::Error e, qulonglong id, const QByteArray& text);
	void captchaReported(Antigate::Error e, qulonglong id);
	void balanceReady(Antigate::Error e, double balance);
	void systemLoadReady(Antigate::Error e, int waiting, double load, double minbid, double avgtime);
	void loginCheckFinished(Antigate::Error e);
	void fundsTransferred(Antigate::Error e);
	void proxiesReady(Antigate::Error e, const QByteArray& list);

private:
	Q_DISABLE_COPY(Client)
	Q_DECLARE_PRIVATE(Client)
	QScopedPointer<ClientPrivate> d_ptr;

	Q_PRIVATE_SLOT(d_func(), void _q_captchaSubmitted(void))
	Q_PRIVATE_SLOT(d_func(), void _q_captchaQueried(void))
	Q_PRIVATE_SLOT(d_func(), void _q_captchaReported(void))
	Q_PRIVATE_SLOT(d_func(), void _q_balanceReady(void))
	Q_PRIVATE_SLOT(d_func(), void _q_sysLoadReady(void))
	Q_PRIVATE_SLOT(d_func(), void _q_loginCheckReady(void))
	Q_PRIVATE_SLOT(d_func(), void _q_transferReady(void))
	Q_PRIVATE_SLOT(d_func(), void _q_proxiesReady(void))
};

}

#endif // CLIENT_H
