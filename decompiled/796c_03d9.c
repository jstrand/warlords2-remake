
void __cdecl16far list_carried_items(uint param_1,int param_2,int param_3,int *param_4)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  uint unaff_DI;
  int iVar5;
  
  bVar1 = false;
  DAT_451b_08ce = 0;
  if (param_1 < 8) {
                    /* WARNING: Switch is manually overridden */
    switch((undefined4)*(undefined2 *)(param_1 * 2 + 0x6fb)) {
    case 0x79abe:
      DAT_451b_08ce = 0;
      for (iVar5 = 0; iVar5 < 0x1e; iVar5 = iVar5 + 1) {
        ((undefined2 *)&DAT_451b_0c9c)[iVar5] = 0;
        ((undefined1 *)&DAT_451b_08dc)[iVar5 * 0x20] = 0;
      }
      break;
    case 0x79ae5:
      iVar5 = army_count;
      do {
        iVar2 = iVar5 + -1;
        if (iVar5 == 0) break;
        iVar5 = iVar2;
      } while ((param_4._2_2_ != 0x2e1d) || ((int *)(iVar2 * 0x16 + -0x7780) != (int *)param_4));
      iVar5 = *param_4;
      iVar4 = ((int *)param_4)[1];
      for (iVar3 = 0; iVar3 < 0x16; iVar3 = iVar3 + 1) {
        if ((*(char *)(iVar3 * 0x1d + 0xcff) == '\x03') && (*(int *)(iVar3 * 0x1d + 0xd00) == iVar2)
           ) {
          ((undefined2 *)&DAT_451b_0c9c)[DAT_451b_08ce] = iVar3;
          strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + DAT_451b_08ce * 0x20),
                 (char *)CONCAT22(0x3c04,(char *)(iVar3 * 0x1d + 0xce9)));
          DAT_451b_08ce = DAT_451b_08ce + 1;
        }
      }
      for (iVar2 = 0; iVar2 < 0x16; iVar2 = iVar2 + 1) {
        if (((*(int *)(iVar2 * 0x1d + 0xd02) == iVar5) && (*(int *)(iVar2 * 0x1d + 0xd04) == iVar4))
           && (*(char *)(iVar2 * 0x1d + 0xcff) == '\x01')) {
          ((undefined2 *)&DAT_451b_0c9c)[DAT_451b_08ce] = iVar2;
          strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + DAT_451b_08ce * 0x20),
                 (char *)CONCAT22(0x3c04,(char *)(iVar2 * 0x1d + 0xce9)));
          DAT_451b_08ce = DAT_451b_08ce + 1;
        }
      }
      break;
    case 0x79bf1:
      iVar5 = army_count;
      do {
        iVar2 = iVar5 + -1;
        if (iVar5 == 0) break;
        iVar5 = iVar2;
      } while ((param_4._2_2_ != 0x2e1d) || ((int *)(iVar2 * 0x16 + -0x7780) != (int *)param_4));
      for (iVar5 = 0; iVar5 < 0x16; iVar5 = iVar5 + 1) {
        if ((*(char *)(iVar5 * 0x1d + 0xcff) == '\x03') && (*(int *)(iVar5 * 0x1d + 0xd00) == iVar2)
           ) {
          ((undefined2 *)&DAT_451b_0c9c)[DAT_451b_08ce] = iVar5;
          strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + DAT_451b_08ce * 0x20),
                 (char *)CONCAT22(0x3c04,(char *)(iVar5 * 0x1d + 0xce9)));
          DAT_451b_08ce = DAT_451b_08ce + 1;
        }
      }
      break;
    case 0x79c7c:
      DAT_451b_08ce = 0;
      for (iVar5 = 0; iVar5 < 0x16; iVar5 = iVar5 + 1) {
        if (((*(int *)(iVar5 * 0x1d + 0xd02) == param_2) &&
            (*(int *)(iVar5 * 0x1d + 0xd04) == param_3)) &&
           (*(char *)(iVar5 * 0x1d + 0xcff) == '\x01')) {
          ((undefined2 *)&DAT_451b_0c9c)[DAT_451b_08ce] = iVar5;
          strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + DAT_451b_08ce * 0x20),
                 (char *)CONCAT22(0x3c04,(char *)(iVar5 * 0x1d + 0xce9)));
          DAT_451b_08ce = DAT_451b_08ce + 1;
        }
      }
      break;
    case 0x79cf6:
      DAT_451b_08ce = 0;
      iVar5 = 0;
      strcpy(&DAT_451b_1d28,(char *)0x412515c6);
      while ((!bVar1 && (iVar5 < 0x1e))) {
        sprintf(&DAT_451b_1cd8,(char *)0x412515ca,(int)(char)((undefined1 *)&DAT_451b_1d28)[param_2]
                ,iVar5);
        iVar2 = FUN_7ecb_0375((undefined1 *)&DAT_451b_1cd8,0x4125);
        if (iVar2 == 0) {
          bVar1 = true;
        }
        else {
          iVar2 = open(&DAT_451b_1cd8,2);
          read(iVar2,(void *)0x284125,unaff_DI);
          for (iVar4 = 0; iVar4 < 0x28; iVar4 = iVar4 + 1) {
            if (((undefined1 *)&DAT_451b_1cd8)[iVar4] == '\r') {
              ((undefined1 *)&DAT_451b_1cd8)[iVar4] = 0;
              break;
            }
          }
          close(iVar2);
          ((undefined2 *)&DAT_451b_0c9c)[DAT_451b_08ce] = iVar5;
          strcpy((char *)CONCAT22(0x4125,(undefined1 *)&DAT_451b_08dc + DAT_451b_08ce * 0x20),
                 &DAT_451b_1cd8);
          DAT_451b_08ce = DAT_451b_08ce + 1;
          iVar5 = iVar5 + 1;
        }
      }
    }
  }
  return;
}


