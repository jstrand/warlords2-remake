
void __cdecl16far FUN_67cc_10d7(int param_1)

{
  undefined2 uVar1;
  int iVar2;
  int iVar3;
  
  if (7 < param_1) {
    return;
  }
  iVar2 = 0;
  for (iVar3 = 0; iVar3 < 8; iVar3 = iVar3 + 1) {
    if (*(int *)(iVar3 * 2 + 0x137) != 0) {
      iVar2 = iVar2 + 1;
    }
  }
  if (iVar2 != 1) {
    FUN_828e_03f9();
    iVar2 = FUN_828e_044a(*(undefined2 *)(param_1 * 0x14 + 0x193),
                          *(undefined2 *)(param_1 * 0x14 + 0x195));
    if ((-1 < iVar2) &&
       (*(char *)(*(byte *)(*(int *)(iVar2 * 0x41 + 0x157f) * 0xe0 +
                           *(int *)(iVar2 * 0x41 + 0x157d) * 2) + 0x710) != '\n')) {
      iVar2 = -1;
    }
    if ((((undefined2 *)&side_city_count)[param_1] != 0) &&
       ((iVar2 < 0 || (*(char *)(iVar2 * 0x41 + 0x1592) != param_1)))) {
      iVar2 = FUN_828e_04fa(*(undefined2 *)(param_1 * 0x14 + 0x189),
                            *(undefined2 *)(param_1 * 0x14 + 0x18b),param_1,0);
      if (iVar2 < 0) {
        return;
      }
      *(undefined2 *)(param_1 * 0x14 + 0x189) = *(undefined2 *)(iVar2 * 0x41 + 0x157d);
      uVar1 = *(undefined2 *)(iVar2 * 0x41 + 0x157f);
      goto LAB_67cc_1137;
    }
  }
  *(undefined2 *)(param_1 * 0x14 + 0x189) = *(undefined2 *)(param_1 * 0x14 + 0x193);
  uVar1 = *(undefined2 *)(param_1 * 0x14 + 0x195);
LAB_67cc_1137:
  *(undefined2 *)(param_1 * 0x14 + 0x18b) = uVar1;
  return;
}


