#ifndef ANTIGATE_H
#define ANTIGATE_H

#include <QtCore/QtGlobal>
#include <QtCore/QMetaType>

#if defined(ANTIGATE_LIBRARY)
#  define ANTIGATE_EXPORT Q_DECL_EXPORT
#elif defined(ANTIGATE_HIDDEN)
#  define ANTIGATE_EXPORT Q_DECL_HIDDEN
#else
#  define ANTIGATE_EXPORT Q_DECL_IMPORT
#endif

namespace Antigate {

enum Error {
	ErrorOK                 = 0,
	ErrorKeyNotExists       = 1,
	ErrorNoSlots            = 2,
	ErrorZeroCaptchaSize    = 3,
	ErrorTooBigCaptchaSize  = 4,
	ErrorWrongExtension     = 5,
	ErrorIPNotAllowed       = 6,
	ErrorWrongIDFormat      = 7,
	ErrorNoSuchID           = 8,
	ErrorURLMethodForbidden = 9,
	ErrorNotReady           = 10,
	ErrorBalance            = 11,
	ErrorNotExists          = 12,
	ErrorWrongTransferKey   = 13,
	ErrorIncorrectAmount    = 14,
	ErrorNoAccess           = 15,
	ErrorReportRecorded     = 0,
	ErrorLoginExists        = 0,
	ErrorTransferComplete   = 0,
	ErrorNetwork            = 200,
	ErrorUnknown            = 201
};

enum CaptchaFormat {
	FormatJPEG = 0,
	FormatGIF  = 1,
	FormatPNG  = 2
};

enum ProxyType {
	ProxyAny         = 0,
	ProxyTransparent = 1,
	ProxyAnonymous   = 2
};

class Client;

ANTIGATE_EXPORT QDebug operator<<(QDebug dbg, const Antigate::Error& v);
ANTIGATE_EXPORT QDebug operator<<(QDebug dbg, const Antigate::CaptchaFormat& v);
ANTIGATE_EXPORT QDebug operator<<(QDebug dbg, const Antigate::ProxyType& v);

}

Q_DECLARE_METATYPE(Antigate::Error)
Q_DECLARE_METATYPE(Antigate::CaptchaFormat)
Q_DECLARE_METATYPE(Antigate::ProxyType)

#endif // ANTIGATE_H
