
void __cdecl16far FUN_513d_0331(void)

{
  bool bVar1;
  bool bVar2;
  int iVar3;
  int iVar4;
  int unaff_DI;
  undefined2 uVar5;
  int local_6;
  int local_4;
  
  local_6 = 0;
  bVar2 = false;
  while (!bVar2) {
    unaff_DI = dice(1,0x66,5);
    local_4 = dice(1,0x92,5);
    bVar1 = true;
    for (iVar4 = 0; iVar4 < 4; iVar4 = iVar4 + 1) {
      uVar5 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
      iVar3 = (int)DAT_451b_002a;
      if ((((*(char *)(iVar3 + (local_4 + *(int *)(iVar4 * 2 + 0x2cc)) * 0x70 + unaff_DI +
                       *(int *)(iVar4 * 2 + 0x2c4) + 0x6120) == '\x03') ||
           (*(char *)(iVar3 + (local_4 + *(int *)(iVar4 * 2 + 0x2cc)) * 0x70 + unaff_DI +
                      *(int *)(iVar4 * 2 + 0x2c4) + 0x6120) == '\x02')) ||
          (*(char *)(iVar3 + (local_4 + *(int *)(iVar4 * 2 + 0x2cc)) * 0x70 + unaff_DI +
                     *(int *)(iVar4 * 2 + 0x2c4) + 0x6120) == '\n')) ||
         ((*(char *)(iVar3 + (local_4 + *(int *)(iVar4 * 2 + 0x2cc)) * 0x70 + unaff_DI +
                     *(int *)(iVar4 * 2 + 0x2c4) + 0x6120) == '\x06' ||
          (*(char *)(iVar3 + (local_4 + *(int *)(iVar4 * 2 + 0x2cc)) * 0x70 + unaff_DI +
                     *(int *)(iVar4 * 2 + 0x2c4) + 0x6120) == '\x05')))) {
        bVar1 = false;
      }
      iVar3 = terrain_near(unaff_DI + *(int *)(iVar4 * 2 + 0x2c4),
                           local_4 + *(int *)(iVar4 * 2 + 0x2cc),10);
      if (iVar3 != 0) {
        bVar1 = false;
      }
      iVar3 = terrain_near(unaff_DI + *(int *)(iVar4 * 2 + 0x2c4) + 1,
                           local_4 + *(int *)(iVar4 * 2 + 0x2cc) + 1,10);
      if (iVar3 != 0) {
        bVar1 = false;
      }
      iVar3 = terrain_near(unaff_DI + *(int *)(iVar4 * 2 + 0x2c4) + -1,
                           local_4 + *(int *)(iVar4 * 2 + 0x2cc) + -1,10);
      if (iVar3 != 0) {
        bVar1 = false;
      }
    }
    if (bVar1) {
      if (local_6 < 4) {
        iVar4 = terrain_near(unaff_DI,local_4,3);
        if (iVar4 != 0) {
          bVar2 = true;
        }
        iVar4 = terrain_near(unaff_DI + 1,local_4,3);
        if (iVar4 != 0) {
          bVar2 = true;
        }
        iVar4 = terrain_near(unaff_DI,local_4 + 1,3);
        if (iVar4 != 0) {
          bVar2 = true;
        }
        iVar4 = terrain_near(unaff_DI + 1,local_4 + 1,3);
        if (iVar4 != 0) {
          bVar2 = true;
        }
        local_6 = local_6 + 1;
      }
      else {
        bVar2 = true;
      }
    }
  }
  *(int *)(city_count * 0x41 + 0x157d) = unaff_DI;
  *(int *)(city_count * 0x41 + 0x157f) = local_4;
  city_count = city_count + 1;
  for (iVar4 = 0; iVar4 < 4; iVar4 = iVar4 + 1) {
    *(undefined1 *)
     ((int)DAT_451b_002a + (local_4 + *(int *)(iVar4 * 2 + 0x2cc)) * 0x70 + unaff_DI +
      *(int *)(iVar4 * 2 + 0x2c4) + 0x6120) = 10;
    FUN_4bed_03aa(unaff_DI + *(int *)(iVar4 * 2 + 0x2c4),local_4 + *(int *)(iVar4 * 2 + 0x2cc));
  }
  return;
}


