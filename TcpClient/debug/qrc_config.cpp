/****************************************************************************
** Resource object code
**
** Created by: The Resource Compiler for Qt version 5.14.2
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

static const unsigned char qt_resource_data[] = {
  // D:/Software/Qt/Qt_project/project1/TcpClient/clinet.config
  0x0,0x0,0x0,0x11,
  0x31,
  0x32,0x37,0x2e,0x30,0x2e,0x30,0x2e,0x31,0xd,0xa,0x38,0x38,0x38,0x38,0xd,0xa,
  
  
};

static const unsigned char qt_resource_name[] = {
  // clinet.config
  0x0,0xd,
  0x1,0x6,0xd5,0xb7,
  0x0,0x63,
  0x0,0x6c,0x0,0x69,0x0,0x6e,0x0,0x65,0x0,0x74,0x0,0x2e,0x0,0x63,0x0,0x6f,0x0,0x6e,0x0,0x66,0x0,0x69,0x0,0x67,
  
};

static const unsigned char qt_resource_struct[] = {
  // :
  0x0,0x0,0x0,0x0,0x0,0x2,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x1,
0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,
  // :/clinet.config
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x1,0x0,0x0,0x0,0x0,
0x0,0x0,0x1,0x8d,0x6e,0x67,0xc5,0x8e,

};

#ifdef QT_NAMESPACE
#  define QT_RCC_PREPEND_NAMESPACE(name) ::QT_NAMESPACE::name
#  define QT_RCC_MANGLE_NAMESPACE0(x) x
#  define QT_RCC_MANGLE_NAMESPACE1(a, b) a##_##b
#  define QT_RCC_MANGLE_NAMESPACE2(a, b) QT_RCC_MANGLE_NAMESPACE1(a,b)
#  define QT_RCC_MANGLE_NAMESPACE(name) QT_RCC_MANGLE_NAMESPACE2( \
        QT_RCC_MANGLE_NAMESPACE0(name), QT_RCC_MANGLE_NAMESPACE0(QT_NAMESPACE))
#else
#   define QT_RCC_PREPEND_NAMESPACE(name) name
#   define QT_RCC_MANGLE_NAMESPACE(name) name
#endif

#ifdef QT_NAMESPACE
namespace QT_NAMESPACE {
#endif

bool qRegisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);
bool qUnregisterResourceData(int, const unsigned char *, const unsigned char *, const unsigned char *);

#ifdef QT_NAMESPACE
}
#endif

int QT_RCC_MANGLE_NAMESPACE(qInitResources_config)();
int QT_RCC_MANGLE_NAMESPACE(qInitResources_config)()
{
    int version = 3;
    QT_RCC_PREPEND_NAMESPACE(qRegisterResourceData)
        (version, qt_resource_struct, qt_resource_name, qt_resource_data);
    return 1;
}

int QT_RCC_MANGLE_NAMESPACE(qCleanupResources_config)();
int QT_RCC_MANGLE_NAMESPACE(qCleanupResources_config)()
{
    int version = 3;
    QT_RCC_PREPEND_NAMESPACE(qUnregisterResourceData)
       (version, qt_resource_struct, qt_resource_name, qt_resource_data);
    return 1;
}

namespace {
   struct initializer {
       initializer() { QT_RCC_MANGLE_NAMESPACE(qInitResources_config)(); }
       ~initializer() { QT_RCC_MANGLE_NAMESPACE(qCleanupResources_config)(); }
   } dummy;
}
