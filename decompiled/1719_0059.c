
void __cdecl16far FUN_1719_0059(byte *param_1,undefined2 param_2,byte *param_3)

{
  byte *pbVar1;
  byte bVar2;
  byte *pbVar3;
  code *pcVar4;
  int iVar5;
  uint uVar6;
  int extraout_DX;
  int extraout_DX_00;
  byte *pbVar7;
  byte *pbVar8;
  byte *pbVar9;
  undefined2 uVar10;
  undefined2 uVar11;
  int in_stack_00000014;
  int in_stack_00000016;
  byte *in_stack_00000018;
  byte *in_stack_0000001c;
  byte *in_stack_00000020;
  int in_stack_00000024;
  undefined2 uVar12;
  
  uVar12 = 0x4125;
  pcVar4 = (code *)swi(0x21);
  (*pcVar4)();
  uVar10 = (undefined2)((ulong)param_1 >> 0x10);
  pbVar9 = (byte *)param_1;
  uVar11 = (undefined2)((ulong)param_3 >> 0x10);
  pbVar7 = (byte *)param_3;
  iVar5 = in_stack_00000014;
  while( true ) {
    while (iVar5 != 0) {
      pbVar8 = pbVar7 + 1;
      bVar2 = *pbVar7;
      if ((bVar2 & 0x80) == 0) {
        for (uVar6 = (uint)(byte)(bVar2 + 1); uVar6 != 0; uVar6 = uVar6 - 1) {
          pbVar3 = pbVar9;
          pbVar9 = pbVar9 + 1;
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 1;
          *pbVar3 = *pbVar1;
        }
        FUN_1719_000a(uVar12);
        iVar5 = extraout_DX_00;
        pbVar7 = pbVar8;
      }
      else {
        iVar5 = pbVar7[2] + 1;
        pbVar8 = pbVar9 + CONCAT11(bVar2,*pbVar8);
        do {
          pbVar3 = pbVar9;
          pbVar9 = pbVar9 + 1;
          pbVar1 = pbVar8;
          pbVar8 = pbVar8 + 1;
          *pbVar3 = *pbVar1;
          iVar5 = iVar5 + -1;
        } while (iVar5 != 0);
        FUN_1719_000a(uVar12);
        iVar5 = extraout_DX;
        pbVar7 = pbVar7 + 3;
      }
    }
    in_stack_00000016 = in_stack_00000016 + -1;
    if (in_stack_00000016 == 0) break;
    in_stack_00000024 = in_stack_00000024 + 1;
    uVar10 = (undefined2)((ulong)in_stack_00000018 >> 0x10);
    pbVar9 = (byte *)in_stack_00000018;
    iVar5 = in_stack_00000014;
    if (in_stack_00000024 != 1) {
      uVar10 = (undefined2)((ulong)in_stack_0000001c >> 0x10);
      pbVar9 = (byte *)in_stack_0000001c;
      if (in_stack_00000024 != 2) {
        uVar10 = (undefined2)((ulong)in_stack_00000020 >> 0x10);
        pbVar9 = (byte *)in_stack_00000020;
      }
    }
  }
  return;
}


