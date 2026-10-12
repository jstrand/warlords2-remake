
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl16far FUN_22bf_098d(int param_1,int param_2)

{
  long lVar1;
  undefined2 uVar2;
  undefined2 uVar3;
  undefined2 uVar4;
  int iVar5;
  uint uVar6;
  undefined2 uVar7;
  
  uVar7 = 0x22bf;
  uVar6 = param_1 - DAT_451b_2fba;
  param_2 = param_2 - DAT_451b_2fb8;
  lVar1 = (long)(int)(uVar6 & 7) * (long)(DAT_4125_35b9 + 8);
  uVar4 = (undefined2)((ulong)lVar1 >> 0x10);
  uVar2 = (undefined2)lVar1;
  if ((_DAT_4125_363d != 0) && ((DAT_4125_3668 & 10) == 0)) {
    for (iVar5 = 0; iVar5 < (int)(uint)DAT_4125_3667; iVar5 = iVar5 + 1) {
      uVar7 = 0x1d28;
      uVar3 = mem_lock(((undefined2 *)&DAT_4125_363d)[iVar5 * 2],
                       *(undefined2 *)((undefined *)&DAT_4125_363f + iVar5 * 4));
      ((undefined2 *)&DAT_4125_364f)[iVar5 * 2] = uVar4;
      ((undefined2 *)&DAT_4125_364d)[iVar5 * 2] = uVar3;
    }
  }
  (*DAT_451b_2a68)(uVar7,0x35bd,0x4125,uVar2,0,DAT_4125_3670 + 0xfU & 0xfff0,DAT_4125_3674,0x342a,
                   0x4125,uVar6 & 0xfff8,param_2,0x3633,0x4125,uVar2,0);
  return;
}


