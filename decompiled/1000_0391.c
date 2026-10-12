
undefined2 __cdecl16near sound_driver_present(void)

{
  code *pcVar1;
  int iVar2;
  undefined2 uVar3;
  
  uVar3 = (undefined2)((ulong)uRam00000198 >> 0x10);
  iVar2 = (int)uRam00000198;
  if (iVar2 == 0) {
LAB_1000_03e3:
    uVar3 = 0;
  }
  else {
    if ((*(int *)(iVar2 + -6) == 0x494d) && (*(int *)(iVar2 + -4) == 0x4944)) {
      pcVar1 = (code *)swi(0x66);
      iVar2 = (*pcVar1)(0x4125);
      if (iVar2 == 0) goto LAB_1000_03e3;
    }
    else if ((*(int *)(iVar2 + -6) != 0x454b) || (*(int *)(iVar2 + -4) != 0x4e52))
    goto LAB_1000_03e3;
    uVar3 = 1;
  }
  return uVar3;
}


