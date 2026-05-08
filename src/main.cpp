#include <windows.h>

#include "SampleManager.h"
#include "ProductionLine.h"
#include "OrderManager.h"
#include "Monitor.h"
#include "ShippingManager.h"
#include "MenuUI.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    SampleManager   sampleMgr;
    ProductionLine  productionLine;
    OrderManager    orderMgr(sampleMgr, productionLine);
    Monitor         monitor(orderMgr, sampleMgr);
    ShippingManager shippingMgr(orderMgr);
    MenuUI          ui(sampleMgr, orderMgr, productionLine, monitor, shippingMgr);

    ui.run();
    return 0;
}
