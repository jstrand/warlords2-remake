
void __cdecl16far diplomatic_rating(int *param_1,undefined4 param_2,undefined2 *param_3)

{
  undefined2 uVar1;
  int iVar2;
  undefined2 *puVar3;
  int iVar4;
  undefined2 uVar5;
  
  iVar4 = 0;
  iVar2 = 0;
  while( true ) {
    uVar5 = (undefined2)((ulong)param_2 >> 0x10);
    if (7 < iVar4) break;
    if (*(int *)(iVar4 * 2 + 0x137) != 0) {
      *(int *)((int)param_2 + iVar2 * 2) = iVar4;
      iVar2 = iVar2 + 1;
    }
    iVar4 = iVar4 + 1;
  }
  *param_1 = iVar2;
  for (iVar2 = 1; iVar4 = iVar2, iVar2 < *param_1; iVar2 = iVar2 + 1) {
    for (; (*(int *)(*(int *)((int)param_2 + iVar4 * 2) * 2 + 0x10e3) <
            *(int *)(*(int *)((int)param_2 + (iVar4 + -1) * 2) * 2 + 0x10e3) && (0 < iVar4));
        iVar4 = iVar4 + -1) {
      uVar1 = *(undefined2 *)((int)param_2 + iVar4 * 2);
      *(undefined2 *)((int)param_2 + iVar4 * 2) = *(undefined2 *)((int)param_2 + (iVar4 + -1) * 2);
      *(undefined2 *)((int)param_2 + (iVar4 + -1) * 2) = uVar1;
    }
  }
  if (*param_1 - 1U < 8) {
                    /* WARNING: Switch is manually overridden */
    puVar3 = (undefined2 *)param_3;
    uVar5 = (undefined2)((ulong)param_3 >> 0x10);
    switch((undefined4)*(undefined2 *)((*param_1 - 1U) * 2 + 0xcb7)) {
    case 0x49099:
      *param_3 = 0;
      break;
    case 0x490a4:
      *param_3 = 0;
      puVar3[1] = 7;
      break;
    case 0x490b5:
      *param_3 = 0;
      puVar3[1] = 3;
      puVar3[2] = 7;
      break;
    case 0x490cc:
      *param_3 = 0;
      puVar3[1] = 1;
      puVar3[2] = 5;
      puVar3[3] = 7;
      break;
    case 0x490e9:
      *param_3 = 0;
      puVar3[1] = 1;
      puVar3[2] = 3;
      puVar3[3] = 5;
      puVar3[4] = 7;
      break;
    case 0x4910c:
      *param_3 = 0;
      puVar3[1] = 1;
      puVar3[2] = 3;
      puVar3[3] = 5;
      puVar3[4] = 6;
      puVar3[5] = 7;
      break;
    case 0x49134:
      *param_3 = 0;
      puVar3[1] = 1;
      puVar3[2] = 3;
      puVar3[3] = 4;
      puVar3[4] = 5;
      puVar3[5] = 6;
      puVar3[6] = 7;
      break;
    case 0x49162:
      *param_3 = 0;
      puVar3[1] = 1;
      puVar3[2] = 2;
      puVar3[3] = 3;
      puVar3[4] = 4;
      puVar3[5] = 5;
      puVar3[6] = 6;
      puVar3[7] = 7;
    }
  }
  return;
}


