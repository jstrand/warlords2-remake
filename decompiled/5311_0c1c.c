
undefined2 __cdecl16far
FUN_5311_0c1c(undefined4 param_1,int param_2,int param_3,int param_4,int param_5,int param_6)

{
  byte *pbVar1;
  char cVar2;
  int iVar3;
  undefined2 uVar4;
  int iVar5;
  byte *pbVar6;
  int iVar7;
  undefined2 unaff_SS;
  char local_de [200];
  int local_16;
  int local_14;
  int local_12;
  int local_10;
  undefined2 local_e;
  undefined2 local_c;
  char *local_a;
  int local_4;
  
  iVar7 = 0;
  local_16 = param_2;
  local_14 = param_3;
  local_12 = param_4;
  local_10 = param_5;
  local_a = local_de;
  local_c = 1;
  local_e = 0;
  iVar3 = pathfind(&local_16);
  if (iVar3 == 0) {
    uVar4 = 0;
  }
  else {
    local_4 = param_2;
    iVar3 = 0;
    while ((((iVar3 < 200 &&
             (((iVar7 = iVar7 + 1, local_4 != param_4 || (param_3 != param_5)) &&
              (local_de[iVar3] != -1)))) && (iVar5 = FUN_1a8b_0b4d(&local_4), iVar5 != 0)) &&
           (((*(byte *)(param_3 * 0x70 + local_4) & 0x1f) == 0 || (param_6 == 0))))) {
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar5 = (int)DAT_451b_002a;
      if (((*(char *)(iVar5 + param_3 * 0x70 + local_4 + 0x6120) == '\x03') ||
          ((*(char *)(iVar5 + param_3 * 0x70 + local_4 + 0x6120) == '\x02' ||
           (*(char *)(iVar5 + param_3 * 0x70 + local_4 + 0x6120) == '\x01')))) &&
         ((cVar2 = *(char *)(*(byte *)(param_3 * 0xe0 + local_4 * 2) + 0x710), cVar2 != '\x03' &&
          ((cVar2 != '\x02' && (cVar2 != '\x01')))))) {
        DAT_3c04_017d = local_4;
        DAT_3c04_017f = param_3;
        FUN_8611_006c();
        FUN_177b_0020(local_4,param_3,0xf0001);
        FUN_177b_005f();
      }
      uVar4 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar5 = (int)DAT_451b_002a;
      if (((*(char *)(iVar5 + param_3 * 0x70 + local_4 + 0x6120) != '\x03') &&
          (*(char *)(iVar5 + param_3 * 0x70 + local_4 + 0x6120) != '\x02')) &&
         (*(char *)(iVar5 + param_3 * 0x70 + local_4 + 0x6120) != '\x01')) {
        pbVar6 = (byte *)(param_3 * 0x70 + local_4);
        pbVar1 = pbVar6;
        *pbVar1 = *pbVar1 & 0xe0;
        pbVar1 = pbVar6;
        *pbVar1 = *pbVar1 | 1;
        FUN_4bed_03aa(local_4,param_3);
      }
      if (5 < iVar7) {
        iVar7 = 0;
        FUN_5311_0f4b(param_1,local_4);
      }
      iVar3 = iVar3 + 1;
    }
    uVar4 = 1;
  }
  return uVar4;
}


