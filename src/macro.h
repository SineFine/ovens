#ifndef MYDEFINE_H
#define MYDEFINE_H

#define WHAT(x)                                                           \
  std::cerr << "!! Error occure in: "                                     \
            << "function( " << __FUNCTION__ << " ) \\ line( " << __LINE__ \
            << " ) !!n"                                                   \
            << "!! Message: " << (x) << " !!" << std::endl;

#define WHEREIS()                                                         \
  std::cout << "{{ A am in: "                                             \
            << "function( " << __FUNCTION__ << " ) \\ line( " << __LINE__ \
            << " ) }}\n"

#define WARRNING(x)                                                       \
  std::cerr << "[[ Warrning: "                                            \
            << "function( " << __FUNCTION__ << " ) \\ line( " << __LINE__ \
            << " ) ]]\n"                                                  \
            << "[[ Message: " << (x) << " ]]" << std::endl;

#define MESSAGE(x)                                                        \
  std::cerr << "{{ Message: "                                             \
            << "function( " << __FUNCTION__ << " ) \\ line( " << __LINE__ \
            << " ) }}\n"                                                  \
            << "{{ Information: " << (x) << " }}" << std::endl;
#endif
