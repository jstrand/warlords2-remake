
void __cdecl16far FUN_25de_07ff(void)

{
  uint uVar1;
  int iVar2;
  uint uVar3;
  uint uVar4;
  bool bVar5;
  uint *in_CX;
  uint *puVar6;
  uint uVar7;
  undefined2 unaff_SS;
  
  bVar5 = false;
  iVar2 = *(int *)&DAT_4125_0110;
  uVar3 = *(uint *)&DAT_4125_0124;
  puVar6 = (uint *)&stack0xfffe;
  while (puVar6 < in_CX) {
    uVar7 = *puVar6 >> 1;
    if (uVar7 == 0) break;
    if (((((*puVar6 & 1) == 0) && (uVar4 = puVar6[2], uVar4 < uVar3)) && (puVar6[1] == 0)) &&
       (*(int *)0x0 == iVar2)) {
      bVar5 = true;
      *(undefined2 *)&DAT_4125_0002 = 0;
    }
    puVar6 = (uint *)(uVar7 << 1);
  }
  if (bVar5) {
    while( true ) {
      uVar7 = *puVar6 >> 1;
      if (uVar7 == 0) break;
      if ((((*puVar6 & 1) == 0) && (uVar4 = puVar6[2], uVar4 < uVar3)) &&
         ((puVar6[1] != 0 && ((*(int *)0x0 == iVar2 && (*(int *)&DAT_4125_0002 == 0)))))) {
        LOCK();
        uVar1 = *(uint *)&DAT_4125_0002;
        *(uint *)&DAT_4125_0002 = puVar6[1];
        UNLOCK();
        puVar6[1] = uVar1;
      }
      puVar6 = (uint *)(uVar7 << 1);
    }
  }
  return;
}


