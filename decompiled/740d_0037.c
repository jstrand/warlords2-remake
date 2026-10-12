
/* WARNING (jumptable): Unable to track spacebase fully for stack */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far switchD_7000:2762::caseD_0(void)

{
  int in_AX;
  int iVar1;
  undefined2 uVar2;
  int in_BX;
  uint uVar3;
  int unaff_BP;
  int unaff_SI;
  int unaff_DI;
  undefined2 unaff_SS;
  byte in_CF;
  char *pcVar4;
  char *pcVar5;
  int group;
  
  iVar1 = (in_AX + 0x71d2) - (uint)in_CF;
  pcVar4 = (char *)(in_BX + unaff_DI + -0x1d);
  *pcVar4 = *pcVar4 + (char)iVar1;
  pcVar4 = (char *)(in_BX + unaff_SI + 0x3c04);
  *pcVar4 = *pcVar4 + (char)((uint)in_BX >> 8);
  *(int *)(unaff_BP + -4) = (int)*(char *)(in_BX + 0x710);
  if ((*(byte *)(unaff_DI * 0x70 + unaff_SI) & 0x1f) != 0) {
    *(undefined2 *)(unaff_BP + -4) = 0;
  }
  *(uint *)(unaff_BP + -10) = (uint)(*(byte *)(unaff_DI * 0xe0 + unaff_SI * 2 + 1) >> 7);
  *(uint *)(unaff_BP + -6) = (*(byte *)(unaff_DI * 0xe0 + unaff_SI * 2 + 1) & 0x10) >> 4;
  uVar2 = FUN_18a9_0896();
  *(undefined2 *)(unaff_BP + -0xc) = uVar2;
  if (*(int *)(unaff_BP + 10) != 1) {
    if (*(int *)(unaff_BP + 10) != 2) {
      return;
    }
    if (_LAB_4125_1174 != 0) {
      return;
    }
    iVar1 = FUN_8611_15ff();
    if (iVar1 == 0) {
      return;
    }
    if ((*(int *)(unaff_BP + -6) == 0) ||
       (((opt_view_enemies == 0 && (current_player != *(int *)(unaff_BP + -2))) &&
        ((*(byte *)(unaff_DI * 0xe0 + unaff_SI * 2 + 1) & 0x20) == 0)))) {
      if (*(int *)(unaff_BP + -4) == 10) {
        FUN_740d_1201();
        FUN_828e_044a();
      }
      else {
        if (*(int *)(unaff_BP + -4) != 0xb) {
          if (*(int *)(unaff_BP + -4) == 9) {
            FUN_740d_0a1c();
            _LAB_4125_1174 = 1;
            return;
          }
          FUN_740d_1201();
          if (*(int *)(unaff_BP + -10) == 0) {
            pcVar4 = get_string(0x81,*(int *)(unaff_BP + -4));
            iVar1 = *(int *)(unaff_BP + -4);
            group = 0x80;
            goto LAB_740d_02f4;
          }
          pcVar4 = (char *)0x4125105d;
          pcVar5 = (char *)0x41251058;
          goto code_r0x000743cf;
        }
        iVar1 = FUN_828e_044a();
        *(int *)(unaff_BP + -8) = iVar1;
        if (iVar1 < 0) {
          FUN_740d_1201();
          FUN_828e_04b5();
          auto_str_blessings_quests();
          _LAB_4125_1174 = 1;
          return;
        }
        FUN_740d_1201();
      }
      FUN_740d_0c73();
    }
    else {
      FUN_740d_1201();
      if (((*(byte *)(unaff_DI * 0xe0 + unaff_SI * 2 + 1) & 0x20) == 0) ||
         (current_player == *(int *)(unaff_BP + -2))) {
        FUN_740d_0bad();
        _LAB_4125_1174 = 1;
        return;
      }
      pcVar4 = get_string(0x82,1);
      group = 0x82;
      iVar1 = 0;
LAB_740d_02f4:
      pcVar5 = get_string(group,iVar1);
code_r0x000743cf:
      FUN_740d_10e1(pcVar5,pcVar4);
    }
    _LAB_4125_1174 = 1;
    return;
  }
  uVar3 = *(int *)(unaff_BP + -0xc) - 2;
  if (9 < uVar3) {
    return;
  }
                    /* WARNING: Switch is manually overridden */
  switch((undefined4)*(undefined2 *)(uVar3 * 2 + 0x316)) {
  case 0x741a3:
    FUN_1c8c_0007(0x10001);
    break;
  case 0x741b4:
    FUN_22bf_0af4(unaff_BP + -0x10);
    FUN_8065_0e04(*(undefined4 *)(unaff_BP + -0x10));
    _LAB_4125_1176 = 1;
    break;
  case 0x741d6:
    FUN_828e_044a();
    goto LAB_740d_014c;
  case 0x741ec:
    FUN_828e_044a();
LAB_740d_014c:
    auto_tutorial_hook_3();
    break;
  case 0x74227:
    auto_tutorial_hook_2(0);
    break;
  case 0x74232:
    military_advisor();
    break;
  case 0x7423c:
    auto_tutorial_hook();
    break;
  case 0x74247:
    attack_tile(unaff_SI,unaff_DI);
  }
  return;
}


