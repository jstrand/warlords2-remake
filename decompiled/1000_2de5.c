
uint __vram_copy(int param_1,uint *param_2,uint *param_3)

{
  uint *puVar1;
  uint *puVar2;
  byte bVar3;
  uint uVar4;
  int iVar5;
  uint uVar6;
  uint *puVar7;
  uint *puVar8;
  uint uVar9;
  uint uVar10;
  bool bVar11;
  
  uVar4 = (uint)DAT_4125_3e7c;
  uVar6 = uVar4;
  if (param_1 != 0) {
    uVar9 = (uint)((ulong)param_3 >> 0x10);
    puVar8 = (uint *)param_3;
    uVar10 = (uint)((ulong)param_2 >> 0x10);
    puVar7 = (uint *)param_2;
    bVar11 = puVar7 < puVar8;
    if (bVar11) {
      iVar5 = param_1 + -1;
      uVar6 = iVar5 * 2;
      puVar7 = puVar7 + iVar5;
      puVar8 = puVar8 + iVar5;
    }
    if (uVar4 == 0) {
      for (; param_1 != 0; param_1 = param_1 + -1) {
        puVar2 = puVar8;
        puVar8 = puVar8 + (uint)bVar11 * -2 + 1;
        puVar1 = puVar7;
        puVar7 = puVar7 + (uint)bVar11 * -2 + 1;
        *puVar2 = *puVar1;
      }
    }
    else {
      uVar6 = uVar9;
      if (uVar9 == uVar10) {
        do {
          do {
            bVar3 = in(0x3da);
          } while ((bool)(bVar3 & 1));
          do {
            bVar3 = in(0x3da);
          } while (!(bool)(bVar3 & 1));
          puVar1 = puVar7;
          puVar7 = puVar7 + (uint)bVar11 * -2 + 1;
          uVar6 = *puVar1;
          do {
            bVar3 = in(0x3da);
          } while ((bool)(bVar3 & 1));
          do {
            bVar3 = in(0x3da);
          } while (!(bool)(bVar3 & 1));
          puVar1 = puVar8;
          puVar8 = puVar8 + (uint)bVar11 * -2 + 1;
          *puVar1 = uVar6;
          param_1 = param_1 + -1;
        } while (param_1 != 0);
      }
      else {
        do {
          do {
            bVar3 = in(0x3da);
            uVar6 = uVar6 & 0xff00;
          } while ((bool)(bVar3 & 1));
          do {
            bVar3 = in(0x3da);
            uVar6 = CONCAT11((char)(uVar6 >> 8),bVar3 >> 1 | bVar3 << 7);
          } while (!(bool)(bVar3 & 1));
          puVar2 = puVar8;
          puVar8 = puVar8 + (uint)bVar11 * -2 + 1;
          puVar1 = puVar7;
          puVar7 = puVar7 + (uint)bVar11 * -2 + 1;
          *puVar2 = *puVar1;
          param_1 = param_1 + -1;
        } while (param_1 != 0);
      }
    }
  }
  return uVar6;
}


