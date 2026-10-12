
undefined4 __stdcall16far __long_divmod(uint param_1,uint param_2,uint param_3,uint param_4)

{
  ulong uVar1;
  uint uVar2;
  uint in_CX;
  int iVar3;
  uint uVar4;
  uint uVar5;
  bool bVar6;
  
  if ((param_4 == 0) && ((param_2 == 0 || (param_3 == 0)))) {
    uVar2 = (uint)(CONCAT22(param_2,param_1) / (ulong)param_3);
    if ((in_CX & 2) != 0) {
      uVar2 = (uint)(CONCAT22(param_2,param_1) % (ulong)param_3);
    }
    param_2 = 0;
  }
  else {
    if ((in_CX & 1) == 0) {
      if ((int)param_2 < 0) {
        bVar6 = param_1 != 0;
        param_1 = -param_1;
        param_2 = -(uint)bVar6 - param_2;
        in_CX = in_CX | 0xc;
      }
      if ((int)param_4 < 0) {
        bVar6 = param_3 != 0;
        param_3 = -param_3;
        param_4 = -(uint)bVar6 - param_4;
        in_CX = in_CX ^ 4;
      }
    }
    iVar3 = 0x20;
    uVar5 = 0;
    uVar4 = 0;
    do {
      bVar6 = (int)param_1 < 0;
      param_1 = param_1 * 2;
      uVar1 = (ulong)CONCAT12(bVar6,param_2) << 1;
      param_2 = (uint)uVar1 | (uint)bVar6;
      bVar6 = (uVar1 & 0x10000) != 0;
      uVar1 = (ulong)CONCAT12(bVar6,uVar4) << 1;
      uVar4 = (uint)uVar1 | (uint)bVar6;
      uVar5 = uVar5 << 1 | (uint)((uVar1 & 0x10000) != 0);
      if ((param_4 <= uVar5) && ((param_4 < uVar5 || (param_3 <= uVar4)))) {
        bVar6 = uVar4 < param_3;
        uVar4 = uVar4 - param_3;
        uVar5 = (uVar5 - param_4) - (uint)bVar6;
        param_1 = param_1 + 1;
      }
      iVar3 = iVar3 + -1;
    } while (iVar3 != 0);
    uVar2 = param_1;
    if ((in_CX & 2) != 0) {
      in_CX = in_CX >> 1;
      uVar2 = uVar4;
      param_2 = uVar5;
    }
    if ((in_CX & 4) != 0) {
      bVar6 = uVar2 != 0;
      uVar2 = -uVar2;
      param_2 = -(uint)bVar6 - param_2;
    }
  }
  return CONCAT22(param_2,uVar2);
}


