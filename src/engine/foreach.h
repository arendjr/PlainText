#ifndef FOREACH_H
#define FOREACH_H

#define PT_FE_1(WHAT, X) WHAT(X)
#define PT_FE_2(WHAT, X, ...) WHAT(X) PT_FE_1(WHAT, __VA_ARGS__)
#define PT_FE_3(WHAT, X, ...) WHAT(X) PT_FE_2(WHAT, __VA_ARGS__)
#define PT_FE_4(WHAT, X, ...) WHAT(X) PT_FE_3(WHAT, __VA_ARGS__)
#define PT_FE_5(WHAT, X, ...) WHAT(X) PT_FE_4(WHAT, __VA_ARGS__)
#define PT_FE_6(WHAT, X, ...) WHAT(X) PT_FE_5(WHAT, __VA_ARGS__)
#define PT_FE_7(WHAT, X, ...) WHAT(X) PT_FE_6(WHAT, __VA_ARGS__)
#define PT_FE_8(WHAT, X, ...) WHAT(X) PT_FE_7(WHAT, __VA_ARGS__)
#define PT_FE_9(WHAT, X, ...) WHAT(X) PT_FE_8(WHAT, __VA_ARGS__)
#define PT_FE_10(WHAT, X, ...) WHAT(X) PT_FE_9(WHAT, __VA_ARGS__)
#define PT_FE_11(WHAT, X, ...) WHAT(X) PT_FE_10(WHAT, __VA_ARGS__)
#define PT_FE_12(WHAT, X, ...) WHAT(X) PT_FE_11(WHAT, __VA_ARGS__)
#define PT_FE_13(WHAT, X, ...) WHAT(X) PT_FE_12(WHAT, __VA_ARGS__)
#define PT_FE_14(WHAT, X, ...) WHAT(X) PT_FE_13(WHAT, __VA_ARGS__)
#define PT_FE_15(WHAT, X, ...) WHAT(X) PT_FE_14(WHAT, __VA_ARGS__)
#define PT_FE_16(WHAT, X, ...) WHAT(X) PT_FE_15(WHAT, __VA_ARGS__)
#define PT_FE_17(WHAT, X, ...) WHAT(X) PT_FE_16(WHAT, __VA_ARGS__)
#define PT_FE_18(WHAT, X, ...) WHAT(X) PT_FE_17(WHAT, __VA_ARGS__)
#define PT_FE_19(WHAT, X, ...) WHAT(X) PT_FE_18(WHAT, __VA_ARGS__)
#define PT_FE_20(WHAT, X, ...) WHAT(X) PT_FE_19(WHAT, __VA_ARGS__)
#define PT_FE_21(WHAT, X, ...) WHAT(X) PT_FE_20(WHAT, __VA_ARGS__)
#define PT_FE_22(WHAT, X, ...) WHAT(X) PT_FE_21(WHAT, __VA_ARGS__)
#define PT_FE_23(WHAT, X, ...) WHAT(X) PT_FE_22(WHAT, __VA_ARGS__)
#define PT_FE_24(WHAT, X, ...) WHAT(X) PT_FE_23(WHAT, __VA_ARGS__)
#define PT_FE_25(WHAT, X, ...) WHAT(X) PT_FE_24(WHAT, __VA_ARGS__)
#define PT_FE_26(WHAT, X, ...) WHAT(X) PT_FE_25(WHAT, __VA_ARGS__)
#define PT_FE_27(WHAT, X, ...) WHAT(X) PT_FE_26(WHAT, __VA_ARGS__)
#define PT_FE_28(WHAT, X, ...) WHAT(X) PT_FE_27(WHAT, __VA_ARGS__)
#define PT_FE_29(WHAT, X, ...) WHAT(X) PT_FE_28(WHAT, __VA_ARGS__)
#define PT_FE_30(WHAT, X, ...) WHAT(X) PT_FE_29(WHAT, __VA_ARGS__)

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                  _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                  _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, NAME, ...) NAME
#define FOR_EACH(action, ...) \
    GET_MACRO(__VA_ARGS__, PT_FE_30, PT_FE_29, PT_FE_28, PT_FE_27, PT_FE_26, \
                           PT_FE_25, PT_FE_24, PT_FE_23, PT_FE_22, PT_FE_21, \
                           PT_FE_20, PT_FE_19, PT_FE_18, PT_FE_17, PT_FE_16, \
                           PT_FE_15, PT_FE_14, PT_FE_13, PT_FE_12, PT_FE_11, \
                           PT_FE_10, PT_FE_9, PT_FE_8, PT_FE_7, PT_FE_6, \
                           PT_FE_5, PT_FE_4, PT_FE_3, PT_FE_2, PT_FE_1)(action, __VA_ARGS__)

#endif // FOREACH_H
