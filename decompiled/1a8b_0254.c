
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined2 __cdecl16far
enter_or_attack_city
          (int param_1,undefined2 param_2,undefined2 param_3,undefined2 *param_4,undefined2 *param_5
          )

{
  int *piVar1;
  byte *pbVar2;
  char cVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int *piVar7;
  undefined2 uVar9;
  int *piVar8;
  
  cVar3 = *(char *)(param_1 * 0x41 + 0x1592);
  if (*(char *)(param_1 * 0x41 + 0x1592) == '\x0f') {
    if (DAT_3c04_0159 != 0) {
      FUN_8611_0565();
    }
    auto_ui_being_attacked(param_2,param_3,1);
    if (DAT_3c04_0159 != 0) {
      if (DAT_451b_1f02 == 0) {
        FUN_8611_006c();
        FUN_177b_005f();
      }
      else {
        FUN_8611_0565();
      }
    }
    if (*(char *)(param_1 * 0x41 + 0x1592) == current_player) {
      *(undefined1 *)((int)DAT_451b_018e + param_1 + 0x56) = 1;
    }
  }
  else if (cVar3 != (char)current_player) {
    if ((*(int *)((int)DAT_451b_018e + 0x46) != param_1) || (iVar4 = param_1, _DAT_451b_1efe == 0))
    {
      iVar4 = FUN_623c_1771(param_1,param_2,param_3);
    }
    if (iVar4 != param_1) {
      iVar6 = 8;
      piVar7 = (int *)&DAT_451b_1efe;
      while( true ) {
        piVar8 = piVar7 + -2;
        iVar5 = iVar6 + -1;
        if (iVar6 == 0) break;
        piVar1 = piVar7 + -1;
        iVar6 = iVar5;
        piVar7 = piVar8;
        if (*piVar8 != 0 || *piVar1 != 0) {
          uVar9 = (undefined2)((ulong)*(undefined4 *)piVar8 >> 0x10);
          iVar5 = (int)*(undefined4 *)piVar8;
          pbVar2 = (byte *)(iVar5 + 0xe);
          *pbVar2 = *pbVar2 & 0xf0;
          pbVar2 = (byte *)(iVar5 + 0xe);
          *pbVar2 = *pbVar2 | 1;
          iVar5 = *piVar8;
          pbVar2 = (byte *)(iVar5 + 0xf);
          *pbVar2 = *pbVar2 & 1;
          pbVar2 = (byte *)(iVar5 + 0xf);
          *pbVar2 = *pbVar2 | (char)iVar4 << 1;
        }
      }
      *param_4 = *(undefined2 *)(iVar4 * 0x41 + 0x157d);
      *param_5 = *(undefined2 *)(iVar4 * 0x41 + 0x157f);
      return 1;
    }
    if (DAT_3c04_0159 != 0) {
      FUN_8611_0565();
    }
    auto_ui_being_attacked(param_2,param_3,1);
    if (*(char *)(param_1 * 0x41 + 0x1592) == current_player) {
      FUN_5e97_0000(param_1,(int)cVar3);
    }
    if (DAT_3c04_0159 != 0) {
      if (DAT_451b_1f02 == 0) {
        FUN_8611_006c();
        FUN_177b_005f();
      }
      else {
        FUN_8611_0565();
      }
    }
  }
  iVar4 = 8;
  piVar7 = (int *)&DAT_451b_1efe;
  while( true ) {
    piVar8 = piVar7 + -2;
    iVar6 = iVar4 + -1;
    if (iVar4 == 0) break;
    piVar1 = piVar7 + -1;
    iVar4 = iVar6;
    piVar7 = piVar8;
    if (*piVar8 != 0 || *piVar1 != 0) {
      uVar9 = (undefined2)((ulong)*(undefined4 *)piVar8 >> 0x10);
      pbVar2 = (byte *)((int)*(undefined4 *)piVar8 + 0xe);
      *pbVar2 = *pbVar2 & 0xf0;
      *(byte *)(*piVar8 + 0xf) = *(byte *)(*piVar8 + 0xf) & 1;
    }
  }
  return 0;
}


