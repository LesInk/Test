#ifndef _BANKUI_H_
#define _BANKUI_H_

T_void BankUIStart    (T_word32 formNum);
T_void BankUIUpdate   (T_void);
T_void BankUIEnd      (T_void);
T_void BankUIDeposit  (T_buttonID buttonID);
T_void BankUIWithdraw (T_buttonID buttonID);

#endif
