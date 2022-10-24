//
// Created by Kirill Zhukov on 24.10.2022.
//

#ifndef UNIT_WALLETERRORS_H
#define UNIT_WALLETERRORS_H

namespace operationStatus {
    enum WalletErrorsCode {
        cOk = 0,
        cLowNativeTokenBalance = 1,
        cTokenBalanceNotFound = 2,
        cLowTokenBalance = 3,
        cEmptyListsOfPreviousTransactions = 4
    };
}

#endif //UNIT_WALLETERRORS_H
