#include <cmm_imx6sx.h>
#include <ccm_analog_imx6sx.h>


CCM_SetRootMux(CMM, ccmRootM4ClkSel, ccmRootmuxM4ClkM4PreClk);
CCM_SetRootDivider(CMM, _ccm_root_div_control, )
