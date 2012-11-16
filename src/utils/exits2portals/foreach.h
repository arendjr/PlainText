#ifndef FOREACH_H
#define FOREACH_H

#define GET_MACRO(_1, _2, _3, _4, _5, _6, _7, _8, _9, _10, \
                  _11, _12, _13, _14, _15, _16, _17, _18, _19, _20, \
                  _21, _22, _23, _24, _25, _26, _27, _28, _29, _30, \
                  _31, _32, NAME, ...) NAME


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
#define PT_FE_31(WHAT, X, ...) WHAT(X) PT_FE_30(WHAT, __VA_ARGS__)
#define PT_FE_32(WHAT, X, ...) WHAT(X) PT_FE_31(WHAT, __VA_ARGS__)

#define FOR_EACH(action, ...) \
    GET_MACRO(__VA_ARGS__, PT_FE_32, PT_FE_31, \
                           PT_FE_30, PT_FE_29, PT_FE_28, PT_FE_27, PT_FE_26, \
                           PT_FE_25, PT_FE_24, PT_FE_23, PT_FE_22, PT_FE_21, \
                           PT_FE_20, PT_FE_19, PT_FE_18, PT_FE_17, PT_FE_16, \
                           PT_FE_15, PT_FE_14, PT_FE_13, PT_FE_12, PT_FE_11, \
                           PT_FE_10, PT_FE_9, PT_FE_8, PT_FE_7, PT_FE_6, \
                           PT_FE_5, PT_FE_4, PT_FE_3, PT_FE_2, PT_FE_1)(action, __VA_ARGS__)


#define PT_C_1(X) + 1
#define PT_C_2(X, ...) + 1 PT_C_1(__VA_ARGS__)
#define PT_C_3(X, ...) + 1 PT_C_2(__VA_ARGS__)
#define PT_C_4(X, ...) + 1 PT_C_3(__VA_ARGS__)
#define PT_C_5(X, ...) + 1 PT_C_4(__VA_ARGS__)
#define PT_C_6(X, ...) + 1 PT_C_5(__VA_ARGS__)
#define PT_C_7(X, ...) + 1 PT_C_6(__VA_ARGS__)
#define PT_C_8(X, ...) + 1 PT_C_7(__VA_ARGS__)
#define PT_C_9(X, ...) + 1 PT_C_8(__VA_ARGS__)
#define PT_C_10(X, ...) + 1 PT_C_9(__VA_ARGS__)
#define PT_C_11(X, ...) + 1 PT_C_10(__VA_ARGS__)
#define PT_C_12(X, ...) + 1 PT_C_11(__VA_ARGS__)
#define PT_C_13(X, ...) + 1 PT_C_12(__VA_ARGS__)
#define PT_C_14(X, ...) + 1 PT_C_13(__VA_ARGS__)
#define PT_C_15(X, ...) + 1 PT_C_14(__VA_ARGS__)
#define PT_C_16(X, ...) + 1 PT_C_15(__VA_ARGS__)
#define PT_C_17(X, ...) + 1 PT_C_16(__VA_ARGS__)
#define PT_C_18(X, ...) + 1 PT_C_17(__VA_ARGS__)
#define PT_C_19(X, ...) + 1 PT_C_18(__VA_ARGS__)
#define PT_C_20(X, ...) + 1 PT_C_19(__VA_ARGS__)
#define PT_C_21(X, ...) + 1 PT_C_20(__VA_ARGS__)
#define PT_C_22(X, ...) + 1 PT_C_21(__VA_ARGS__)
#define PT_C_23(X, ...) + 1 PT_C_22(__VA_ARGS__)
#define PT_C_24(X, ...) + 1 PT_C_23(__VA_ARGS__)
#define PT_C_25(X, ...) + 1 PT_C_24(__VA_ARGS__)
#define PT_C_26(X, ...) + 1 PT_C_25(__VA_ARGS__)
#define PT_C_27(X, ...) + 1 PT_C_26(__VA_ARGS__)
#define PT_C_28(X, ...) + 1 PT_C_27(__VA_ARGS__)
#define PT_C_29(X, ...) + 1 PT_C_28(__VA_ARGS__)
#define PT_C_30(X, ...) + 1 PT_C_29(__VA_ARGS__)
#define PT_C_31(X, ...) + 1 PT_C_30(__VA_ARGS__)
#define PT_C_32(X, ...) + 1 PT_C_31(__VA_ARGS__)

#define COUNT(...) \
    (0 GET_MACRO(__VA_ARGS__, PT_C_32, PT_C_31, \
                              PT_C_30, PT_C_29, PT_C_28, PT_C_27, PT_C_26, \
                              PT_C_25, PT_C_24, PT_C_23, PT_C_22, PT_C_21, \
                              PT_C_20, PT_C_19, PT_C_18, PT_C_17, PT_C_16, \
                              PT_C_15, PT_C_14, PT_C_13, PT_C_12, PT_C_11, \
                              PT_C_10, PT_C_9, PT_C_8, PT_C_7, PT_C_6, \
                              PT_C_5, PT_C_4, PT_C_3, PT_C_2, PT_C_1)(__VA_ARGS__))


#define PT_FEC_1(WHAT, C, X) WHAT(X, (C))
#define PT_FEC_2(WHAT, C, X, ...) WHAT(X, (C - 1)) PT_FEC_1(WHAT, C, __VA_ARGS__)
#define PT_FEC_3(WHAT, C, X, ...) WHAT(X, (C - 2)) PT_FEC_2(WHAT, C, __VA_ARGS__)
#define PT_FEC_4(WHAT, C, X, ...) WHAT(X, (C - 3)) PT_FEC_3(WHAT, C, __VA_ARGS__)
#define PT_FEC_5(WHAT, C, X, ...) WHAT(X, (C - 4)) PT_FEC_4(WHAT, C, __VA_ARGS__)
#define PT_FEC_6(WHAT, C, X, ...) WHAT(X, (C - 5)) PT_FEC_5(WHAT, C, __VA_ARGS__)
#define PT_FEC_7(WHAT, C, X, ...) WHAT(X, (C - 6)) PT_FEC_6(WHAT, C, __VA_ARGS__)
#define PT_FEC_8(WHAT, C, X, ...) WHAT(X, (C - 7)) PT_FEC_7(WHAT, C, __VA_ARGS__)
#define PT_FEC_9(WHAT, C, X, ...) WHAT(X, (C - 8)) PT_FEC_8(WHAT, C, __VA_ARGS__)
#define PT_FEC_10(WHAT, C, X, ...) WHAT(X, (C - 9)) PT_FEC_9(WHAT, C, __VA_ARGS__)
#define PT_FEC_11(WHAT, C, X, ...) WHAT(X, (C - 10)) PT_FEC_10(WHAT, C, __VA_ARGS__)
#define PT_FEC_12(WHAT, C, X, ...) WHAT(X, (C - 11)) PT_FEC_11(WHAT, C, __VA_ARGS__)
#define PT_FEC_13(WHAT, C, X, ...) WHAT(X, (C - 12)) PT_FEC_12(WHAT, C, __VA_ARGS__)
#define PT_FEC_14(WHAT, C, X, ...) WHAT(X, (C - 13)) PT_FEC_13(WHAT, C, __VA_ARGS__)
#define PT_FEC_15(WHAT, C, X, ...) WHAT(X, (C - 14)) PT_FEC_14(WHAT, C, __VA_ARGS__)
#define PT_FEC_16(WHAT, C, X, ...) WHAT(X, (C - 15)) PT_FEC_15(WHAT, C, __VA_ARGS__)
#define PT_FEC_17(WHAT, C, X, ...) WHAT(X, (C - 16)) PT_FEC_16(WHAT, C, __VA_ARGS__)
#define PT_FEC_18(WHAT, C, X, ...) WHAT(X, (C - 17)) PT_FEC_17(WHAT, C, __VA_ARGS__)
#define PT_FEC_19(WHAT, C, X, ...) WHAT(X, (C - 18)) PT_FEC_18(WHAT, C, __VA_ARGS__)
#define PT_FEC_20(WHAT, C, X, ...) WHAT(X, (C - 19)) PT_FEC_19(WHAT, C, __VA_ARGS__)
#define PT_FEC_21(WHAT, C, X, ...) WHAT(X, (C - 20)) PT_FEC_20(WHAT, C, __VA_ARGS__)
#define PT_FEC_22(WHAT, C, X, ...) WHAT(X, (C - 21)) PT_FEC_21(WHAT, C, __VA_ARGS__)
#define PT_FEC_23(WHAT, C, X, ...) WHAT(X, (C - 22)) PT_FEC_22(WHAT, C, __VA_ARGS__)
#define PT_FEC_24(WHAT, C, X, ...) WHAT(X, (C - 23)) PT_FEC_23(WHAT, C, __VA_ARGS__)
#define PT_FEC_25(WHAT, C, X, ...) WHAT(X, (C - 24)) PT_FEC_24(WHAT, C, __VA_ARGS__)
#define PT_FEC_26(WHAT, C, X, ...) WHAT(X, (C - 25)) PT_FEC_25(WHAT, C, __VA_ARGS__)
#define PT_FEC_27(WHAT, C, X, ...) WHAT(X, (C - 26)) PT_FEC_26(WHAT, C, __VA_ARGS__)
#define PT_FEC_28(WHAT, C, X, ...) WHAT(X, (C - 27)) PT_FEC_27(WHAT, C, __VA_ARGS__)
#define PT_FEC_29(WHAT, C, X, ...) WHAT(X, (C - 28)) PT_FEC_28(WHAT, C, __VA_ARGS__)
#define PT_FEC_30(WHAT, C, X, ...) WHAT(X, (C - 29)) PT_FEC_29(WHAT, C, __VA_ARGS__)
#define PT_FEC_31(WHAT, C, X, ...) WHAT(X, (C - 30)) PT_FEC_30(WHAT, C, __VA_ARGS__)
#define PT_FEC_32(WHAT, C, X, ...) WHAT(X, (C - 31)) PT_FEC_31(WHAT, C, __VA_ARGS__)

#define FOR_EACH_COUNTED(action, ...) \
    GET_MACRO(__VA_ARGS__, PT_FEC_32, PT_FEC_31, \
                           PT_FEC_30, PT_FEC_29, PT_FEC_28, PT_FEC_27, PT_FEC_26, \
                           PT_FEC_25, PT_FEC_24, PT_FEC_23, PT_FEC_22, PT_FEC_21, \
                           PT_FEC_20, PT_FEC_19, PT_FEC_18, PT_FEC_17, PT_FEC_16, \
                           PT_FEC_15, PT_FEC_14, PT_FEC_13, PT_FEC_12, PT_FEC_11, \
                           PT_FEC_10, PT_FEC_9, PT_FEC_8, PT_FEC_7, PT_FEC_6, \
                           PT_FEC_5, PT_FEC_4, PT_FEC_3, PT_FEC_2, PT_FEC_1) \
             (action, COUNT(__VA_ARGS__) - 1, __VA_ARGS__)

#endif // FOREACH_H
