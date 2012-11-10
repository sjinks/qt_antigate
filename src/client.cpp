#include <QtCore/QDebug>
#include <QtCore/QUrl>
#include <QtNetwork/QHttpMultiPart>
#include <QtNetwork/QHttpPart>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <new>
#include "client.h"
#include "client_p.h"

namespace Antigate {

Q_GLOBAL_STATIC(QNetworkAccessManager, g_nam)

Client::Client(QObject* parent)
	: QObject(parent), d_ptr(new ClientPrivate(this))
{
	static bool init = false;
	if (!init) {
		qRegisterMetaType<Antigate::Error>();
		qRegisterMetaType<Antigate::CaptchaFormat>();
		qRegisterMetaType<Antigate::ProxyType>();
		init = true;
	}
}

Client::~Client(void)
{
}

qulonglong Client::submitCaptcha(const QByteArray& key, const QByteArray& captcha, Antigate::CaptchaFormat format, bool phrase, bool regsense, int numeric, bool calc, int minlen, int maxlen, int isrussian)
{
	Q_D(Client);
	return d->submitCaptcha(key, captcha, format, phrase, regsense, numeric, calc, minlen, maxlen, isrussian);
}

void Client::checkCaptchaStatus(const QByteArray& key, qulonglong id)
{
	Q_D(Client);
	d->checkCaptchaStatus(key, id);
}

void Client::reportCaptcha(const QByteArray& key, qulonglong id)
{
	Q_D(Client);
	d->reportCaptcha(key, id);
}

void Client::queryBalance(const QByteArray& key)
{
	Q_D(Client);
	d->queryBalance(key);
}

void Client::querySystemLoad(void)
{
	Q_D(Client);
	d->querySystemLoad();
}

void Client::checkLoginExists(const QByteArray& tkey, const QByteArray& login)
{
	Q_D(Client);
	d->checkLoginExists(tkey, login);
}

void Client::transferFunds(const QByteArray& tkey, const QByteArray& login, double amount)
{
	Q_D(Client);
	d->transferFunds(tkey, login, amount);
}

void Client::getProxies(const QByteArray& key, ProxyType type)
{
	Q_D(Client);
	d->getProxies(key, type);
}

void ClientPrivate::setNetworkAccessManager(QNetworkAccessManager* mgr)
{
	if (this->m_mgr) {
		this->m_mgr->deleteLater();
	}

	this->m_mgr = mgr;
}

qulonglong ClientPrivate::submitCaptcha(const QByteArray& key, const QByteArray& captcha, Antigate::CaptchaFormat format, bool phrase, bool regsense, int numeric, bool calc, int minlen, int maxlen, int isrussian)
{
	QUrl url(QLatin1String("http://antigate.com/in.php"));
	QNetworkRequest req(url);

	QHttpMultiPart* multipart = new(std::nothrow) QHttpMultiPart(QHttpMultiPart::FormDataType);
	if (!multipart) {
		return 0;
	}

	{
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentDispositionHeader, QLatin1String("form-data; name=\"method\""));
		part.setBody("post");
		multipart->append(part);
	}

	{
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentDispositionHeader, QLatin1String("form-data; name=\"key\""));
		part.setBody(key);
		multipart->append(part);
	}

	{
		QHttpPart part;

		QByteArray x("form-data; name=\"file\"; ");
		switch (format) {
			case Antigate::FormatGIF:
				x.append("filename=\"captcha.gif\"");
				part.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("image/gif"));
				break;

			case Antigate::FormatPNG:
				x.append("filename=\"captcha.png\"");
				part.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("image/png"));
				break;

			case Antigate::FormatJPEG:
			default:
				x.append("filename=\"captcha.jpg\"");
				part.setHeader(QNetworkRequest::ContentTypeHeader, QLatin1String("image/pjpeg"));
				break;
		}

		part.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(x));
		part.setBody(captcha);

		multipart->append(part);
	}

	if (phrase) {
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentDispositionHeader, QLatin1String("form-data; name=\"phrase\""));
		part.setBody("1");
		multipart->append(part);
	}

	if (regsense) {
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentDispositionHeader, QLatin1String("form-data; name=\"regsense\""));
		part.setBody("1");
		multipart->append(part);
	}

	if (numeric > 0 && numeric <= 2) {
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentDispositionHeader, QLatin1String("form-data; name=\"numeric\""));
		part.setBody(QByteArray::number(numeric));
		multipart->append(part);
	}

	if (calc) {
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentDispositionHeader, QLatin1String("form-data; name=\"calc\""));
		part.setBody("1");
		multipart->append(part);
	}

	if (minlen > 0) {
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentDispositionHeader, QLatin1String("form-data; name=\"min_len\""));
		part.setBody(QByteArray::number(minlen));
		multipart->append(part);
	}

	if (maxlen > 0) {
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentDispositionHeader, QLatin1String("form-data; name=\"max_len\""));
		part.setBody(QByteArray::number(maxlen));
		multipart->append(part);
	}

	if (isrussian > 0) {
		QHttpPart part;
		part.setHeader(QNetworkRequest::ContentDispositionHeader, QLatin1String("form-data; name=\"is_russian\""));
		part.setBody(QByteArray::number(isrussian));
		multipart->append(part);
	}

	QNetworkReply* reply = this->mgr()->post(req, multipart);
	multipart->setParent(reply);

	Q_Q(Client);
	QObject::connect(reply, SIGNAL(finished()), q, SLOT(_q_captchaSubmitted()));

	return qulonglong(reply);
}

void ClientPrivate::checkCaptchaStatus(const QByteArray& key, qulonglong id)
{
	QUrl url(QLatin1String("http://antigate.com/res.php?action=get"));
	url.addEncodedQueryItem("key", key.toPercentEncoding());
	url.addEncodedQueryItem("id", QByteArray::number(id));

	QNetworkRequest req(url);
	req.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), id);
	QNetworkReply* reply = this->mgr()->get(req);

	Q_Q(Client);
	QObject::connect(reply, SIGNAL(finished()), q, SLOT(_q_captchaQueried()));
}

void ClientPrivate::reportCaptcha(const QByteArray& key, qulonglong id)
{
	QUrl url(QLatin1String("http://antigate.com/res.php?action=reportbad"));
	url.addEncodedQueryItem("key", key.toPercentEncoding());
	url.addEncodedQueryItem("id", QByteArray::number(id));

	QNetworkRequest req(url);
	req.setAttribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1), id);
	QNetworkReply* reply = this->mgr()->get(req);

	Q_Q(Client);
	QObject::connect(reply, SIGNAL(finished()), q, SLOT(_q_captchaReported()));
}

void ClientPrivate::queryBalance(const QByteArray& key)
{
	QUrl url(QLatin1String("http://antigate.com/res.php?action=getbalance"));
	url.addEncodedQueryItem("key", key.toPercentEncoding());

	QNetworkRequest req(url);
	QNetworkReply* reply = this->mgr()->get(req);

	Q_Q(Client);
	QObject::connect(reply, SIGNAL(finished()), q, SLOT(_q_balanceReady()));
}

void ClientPrivate::querySystemLoad(void)
{
	QUrl url(QLatin1String("http://antigate.com/load.php"));
	QNetworkRequest req(url);
	QNetworkReply* reply = this->mgr()->get(req);

	Q_Q(Client);
	QObject::connect(reply, SIGNAL(finished()), q, SLOT(_q_sysLoadReady()));
}

void ClientPrivate::checkLoginExists(const QByteArray& tkey, const QByteArray& login)
{
	QUrl url(QLatin1String("http://antigate.com/transfer.php"));
	QNetworkRequest req(url);

	QByteArray post;
	post
		.append("action=checklogin&transferkey=")
		.append(tkey.toPercentEncoding())
		.append("&target=")
		.append(login.toPercentEncoding())
	;

	QNetworkReply* reply = this->mgr()->post(req, post);

	Q_Q(Client);
	QObject::connect(reply, SIGNAL(finished()), q, SLOT(_q_loginCheckReady()));
}

void ClientPrivate::transferFunds(const QByteArray &tkey, const QByteArray &login, double amount)
{
	QUrl url(QLatin1String("http://antigate.com/transfer.php"));
	QNetworkRequest req(url);

	QByteArray post;
	post
		.append("action=internaltransfer&transferkey=")
		.append(tkey.toPercentEncoding())
		.append("&target=")
		.append(login.toPercentEncoding())
		.append("&ammount=")
		.append(QByteArray::number(amount).toPercentEncoding())
	;

	QNetworkReply* reply = this->mgr()->post(req, post);

	Q_Q(Client);
	QObject::connect(reply, SIGNAL(finished()), q, SLOT(_q_transferReady()));
}

void ClientPrivate::getProxies(const QByteArray& key, ProxyType type)
{
	QUrl url(QLatin1String("http://antigate.com/res.php?action=getproxy&id=0"));
	url.addEncodedQueryItem("key", key.toPercentEncoding());

	switch (type) {
		case Antigate::ProxyTransparent:
			url.addEncodedQueryItem("is_anonim", "0");
			break;

		case Antigate::ProxyAnonymous:
			url.addEncodedQueryItem("is_anonim", "1");
			break;

		default:
			break;
	}

	QNetworkRequest req(url);
	QNetworkReply* reply = this->mgr()->get(req);

	Q_Q(Client);
	QObject::connect(reply, SIGNAL(finished()), q, SLOT(_q_proxiesReady()));
}

ClientPrivate::ClientPrivate(Client* p)
	: q_ptr(p), m_mgr(0)
{
}

QNetworkAccessManager* ClientPrivate::mgr(void) const
{
	return this->m_mgr ? this->m_mgr : Antigate::g_nam();
}

void ClientPrivate::_q_captchaSubmitted(void)
{
	Q_Q(Client);
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(q->sender());
	Q_ASSERT(reply != 0);

	reply->deleteLater();
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray data = reply->readAll();
		if (data.left(2) == "OK") {
			QByteArray x = data.mid(3);
			qulonglong id = x.toULongLong();
			Q_EMIT q->captchaSubmitted(Antigate::ErrorOK, qulonglong(reply), id);
		}
		else {
			Q_EMIT q->captchaSubmitted(ClientPrivate::string2error(data), qulonglong(reply), 0);
		}

	}
	else {
		Q_EMIT q->captchaSubmitted(Antigate::ErrorNetwork, qulonglong(reply), 0);
	}
}

void ClientPrivate::_q_captchaQueried(void)
{
	Q_Q(Client);
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(q->sender());
	Q_ASSERT(reply != 0);

	qulonglong id = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1)).toULongLong();

	reply->deleteLater();
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray data = reply->readAll();
		if (data.left(2) == "OK") {
			QByteArray text = data.mid(3);
			Q_EMIT q->captchaStatus(Antigate::ErrorOK, id, text);
		}
		else {
			Q_EMIT q->captchaStatus(ClientPrivate::string2error(data), id, QByteArray());
		}
	}
	else {
		Q_EMIT q->captchaStatus(Antigate::ErrorNetwork, id, QByteArray());
	}
}

void ClientPrivate::_q_captchaReported(void)
{
	Q_Q(Client);
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(q->sender());
	Q_ASSERT(reply != 0);

	qulonglong id = reply->request().attribute(static_cast<QNetworkRequest::Attribute>(QNetworkRequest::User + 1)).toULongLong();

	reply->deleteLater();
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray data = reply->readAll();
		Q_EMIT q->captchaReported(ClientPrivate::string2error(data), id);
	}
	else {
		Q_EMIT q->captchaReported(Antigate::ErrorNetwork, id);
	}
}

void ClientPrivate::_q_balanceReady(void)
{
	Q_Q(Client);
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(q->sender());
	Q_ASSERT(reply != 0);

	reply->deleteLater();
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray data = reply->readAll();
		if ("ERROR_" == data.left(6)) {
			Q_EMIT q->balanceReady(ClientPrivate::string2error(data), 0);
		}
		else {
			Q_EMIT q->balanceReady(Antigate::ErrorOK, data.toDouble());
		}
	}
	else {
		Q_EMIT q->balanceReady(Antigate::ErrorNetwork, 0);
	}
}

void ClientPrivate::_q_sysLoadReady(void)
{
	Q_Q(Client);
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(q->sender());
	Q_ASSERT(reply != 0);

	reply->deleteLater();
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray data = reply->readAll();
		QList<QByteArray> items = data.split(' ');

		if (4 == items.size()) {
			int waiting    = items.at(0).toInt();
			double load    = items.at(1).toDouble();
			double minbid  = items.at(2).toDouble();
			double avgtime = items.at(3).toDouble();
			Q_EMIT q->systemLoadReady(Antigate::ErrorOK, waiting, load, minbid, avgtime);
		}
		else {
			Q_EMIT q->systemLoadReady(Antigate::ErrorUnknown, 0, 0, 0, 0);
		}
	}
	else {
		Q_EMIT q->systemLoadReady(Antigate::ErrorNetwork, 0, 0, 0, 0);
	}
}

void ClientPrivate::_q_loginCheckReady(void)
{
	Q_Q(Client);
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(q->sender());
	Q_ASSERT(reply != 0);

	reply->deleteLater();
	Antigate::Error e;
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray data = reply->readAll().trimmed();
		e = ClientPrivate::string2error(data);
	}
	else {
		e = Antigate::ErrorNetwork;
	}

	Q_EMIT q->loginCheckFinished(e);
}

void ClientPrivate::_q_transferReady(void)
{
	Q_Q(Client);
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(q->sender());
	Q_ASSERT(reply != 0);

	reply->deleteLater();
	Antigate::Error e;
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray data = reply->readAll().trimmed();
		e = ClientPrivate::string2error(data);
	}
	else {
		e = Antigate::ErrorNetwork;
	}

	Q_EMIT q->fundsTransferred(e);
}

void ClientPrivate::_q_proxiesReady(void)
{
	Q_Q(Client);
	QNetworkReply* reply = qobject_cast<QNetworkReply*>(q->sender());
	Q_ASSERT(reply != 0);

	reply->deleteLater();
	if (reply->error() == QNetworkReply::NoError) {
		QByteArray data = reply->readAll();
		if ("ERROR_" == data.left(6)) {
			Q_EMIT q->proxiesReady(ClientPrivate::string2error(data), 0);
		}
		else {
			Q_EMIT q->proxiesReady(Antigate::ErrorOK, data);
		}
	}
	else {
		Q_EMIT q->proxiesReady(Antigate::ErrorNetwork, QByteArray());
	}
}

Error ClientPrivate::string2error(const QByteArray& code)
{
	if (code.left(6) == "ERROR_") {
		const QByteArray c = code.mid(6);
		if (c == "KEY_DOES_NOT_EXIST") {
			return Antigate::ErrorKeyNotExists;
		}

		if (c == "NO_SLOT_AVAILABLE") {
			return Antigate::ErrorNoSlots;
		}

		if (c == "ZERO_CAPTCHA_FILESIZE") {
			return Antigate::ErrorZeroCaptchaSize;
		}

		if (c == "TOO_BIG_CAPTCHA_FILESIZE") {
			return Antigate::ErrorTooBigCaptchaSize;
		}

		if (c == "WRONG_FILE_EXTENSION") {
			return Antigate::ErrorWrongExtension;
		}

		if (c == "IP_NOT_ALLOWED") {
			return Antigate::ErrorIPNotAllowed;
		}

		if (c == "WRONG_ID_FORMAT") {
			return Antigate::ErrorWrongIDFormat;
		}

		if (c == "NO_SUCH_CAPCHA_ID") {
			return Antigate::ErrorNoSuchID;
		}

		if (c == "URL_METHOD_FORBIDDEN") {
			return Antigate::ErrorURLMethodForbidden;
		}

		if (c == "ZERO_BALANCE" || code == "BALANCE_NOT_SUFFICIENT") {
			return Antigate::ErrorBalance;
		}

		if (c == "WRONG_TRANSFER_KEY") {
			return Antigate::ErrorWrongTransferKey;
		}

		if (c == "INCORRECT_AMMOUNT") {
			return Antigate::ErrorIncorrectAmount;
		}

		if (c == "TARGET_NOT_EXISTS") {
			return Antigate::ErrorNotExists;
		}

		if (c == "NO_ACCESS") {
			return Antigate::ErrorNoAccess;
		}

		return Antigate::ErrorUnknown;
	}

	if (code.left(3) == "OK_") {
		if (code == "OK_REPORT_RECORDED") {
			return Antigate::ErrorReportRecorded;
		}

		if (code == "OK_EXISTS") {
			return Antigate::ErrorLoginExists;
		}

		if (code == "OK_COMPLETE") {
			return Antigate::ErrorTransferComplete;
		}

		qWarning("Returning ErrorOK for unknown response %s", code.constData());
		return Antigate::ErrorOK;
	}

	if (code == "CAPCHA_NOT_READY") {
		return Antigate::ErrorNotReady;
	}

	if (code == "NOT_EXISTS") {
		return Antigate::ErrorNotExists;
	}

	return Antigate::ErrorUnknown;
}

QDebug operator<<(QDebug dbg, const Antigate::Error& v)
{
	switch (v) {
		case Antigate::ErrorOK:                 dbg << "ErrorOK"; break;
		case Antigate::ErrorKeyNotExists:       dbg << "ErrorKeyNotExists"; break;
		case Antigate::ErrorNoSlots:            dbg << "ErrorNoSlots"; break;
		case Antigate::ErrorZeroCaptchaSize:    dbg << "ErrorZeroCaptchaSize"; break;
		case Antigate::ErrorTooBigCaptchaSize:  dbg << "ErrorTooBigCaptchaSize"; break;
		case Antigate::ErrorWrongExtension:     dbg << "ErrorWrongExtension"; break;
		case Antigate::ErrorIPNotAllowed:       dbg << "ErrorIPNotAllowed"; break;
		case Antigate::ErrorWrongIDFormat:      dbg << "ErrorWrongIDFormat"; break;
		case Antigate::ErrorNoSuchID:           dbg << "ErrorNoSuchID"; break;
		case Antigate::ErrorURLMethodForbidden: dbg << "ErrorURLMethodForbidden"; break;
		case Antigate::ErrorNotReady:           dbg << "ErrorNotReady"; break;
		case Antigate::ErrorBalance:            dbg << "ErrorBalance"; break;
		case Antigate::ErrorNotExists:          dbg << "ErrorNotExists"; break;
		case Antigate::ErrorWrongTransferKey:   dbg << "ErrorWrongTransferKey"; break;
		case Antigate::ErrorIncorrectAmount:    dbg << "ErrorIncorrectAmount"; break;
		case Antigate::ErrorNoAccess:           dbg << "ErrorNoAccess"; break;
		case Antigate::ErrorNetwork:            dbg << "ErrorNetwork"; break;
		case Antigate::ErrorUnknown:            dbg << "ErrorUnknown"; break;
		default:                                dbg << "(unknown error)"; break;
	}

	return dbg;
}

QDebug operator<<(QDebug dbg, const Antigate::CaptchaFormat& v)
{
	switch (v) {
		case Antigate::FormatGIF:  dbg << "FormatGIF"; break;
		case Antigate::FormatJPEG: dbg << "FormatJPEG"; break;
		case Antigate::FormatPNG:  dbg << "FormatPNG"; break;
		default:                   dbg << "(Unknown format)"; break;
	}

	return dbg;
}

QDebug operator<<(QDebug dbg, const Antigate::ProxyType& v)
{
	switch (v) {
		case Antigate::ProxyAny:         dbg << "ProxyAny"; break;
		case Antigate::ProxyTransparent: dbg << "ProxyTransparent"; break;
		case Antigate::ProxyAnonymous:   dbg << "ProxyAnonymous"; break;
		default:                         dbg << "(Unknown proxy type)"; break;
	}

	return dbg;
}

}

#include "moc_client.cpp"
