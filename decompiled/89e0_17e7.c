
void __cdecl16far FUN_89e0_17e7(int param_1)

{
  undefined2 uVar1;
  undefined2 uVar2;
  bool bVar3;
  int iVar4;
  undefined2 uVar5;
  int iVar6;
  undefined2 in_DX;
  int iVar7;
  
  iVar4 = FUN_1b62_0024(current_player);
  if (param_1 != 0) {
    for (iVar7 = 1; iVar7 < DAT_4125_30f6; iVar7 = iVar7 + 1) {
      iVar6 = iVar7;
      if (((int)((undefined2 *)&DAT_451b_28f2)[iVar7] < (int)((undefined2 *)&DAT_451b_28f0)[iVar7])
         || ((((undefined2 *)&DAT_451b_28f2)[iVar7] == ((undefined2 *)&DAT_451b_28f0)[iVar7] &&
             (*(char *)(iVar4 + *(char *)((int)*(undefined4 *)
                                                ((undefined2 *)&DAT_451b_28fe + iVar7 * 2) + 4)) <
              *(char *)(iVar4 + *(char *)((int)*(undefined4 *)
                                                ((undefined2 *)&DAT_451b_2902 + iVar7 * 2) + 4))))))
      {
        while ((0 < iVar6 &&
               (((int)((undefined2 *)&DAT_451b_28f2)[iVar6] <
                 (int)((undefined2 *)&DAT_451b_28f0)[iVar6] ||
                ((((undefined2 *)&DAT_451b_28f2)[iVar6] == ((undefined2 *)&DAT_451b_28f0)[iVar6] &&
                 (*(char *)(iVar4 + *(char *)((int)*(undefined4 *)
                                                    ((undefined2 *)&DAT_451b_28fe + iVar6 * 2) + 4))
                  < *(char *)(iVar4 + *(char *)((int)*(undefined4 *)
                                                      ((undefined2 *)&DAT_451b_2902 + iVar6 * 2) + 4
                                               ))))))))) {
          uVar5 = ((undefined2 *)&DAT_451b_2904)[iVar6 * 2];
          uVar1 = ((undefined2 *)&DAT_451b_2902)[iVar6 * 2];
          uVar2 = ((undefined2 *)&DAT_451b_28fe)[iVar6 * 2];
          ((undefined2 *)&DAT_451b_2904)[iVar6 * 2] =
               *(undefined2 *)((undefined *)&DAT_451b_2900 + iVar6 * 4);
          ((undefined2 *)&DAT_451b_2902)[iVar6 * 2] = uVar2;
          *(undefined2 *)((undefined *)&DAT_451b_2900 + iVar6 * 4) = uVar5;
          ((undefined2 *)&DAT_451b_28fe)[iVar6 * 2] = uVar1;
          uVar5 = ((undefined2 *)&DAT_451b_28f2)[iVar6];
          ((undefined2 *)&DAT_451b_28f2)[iVar6] = ((undefined2 *)&DAT_451b_28f0)[iVar6];
          ((undefined2 *)&DAT_451b_28f0)[iVar6] = uVar5;
          uVar5 = ((undefined2 *)&DAT_451b_28e2)[iVar6];
          ((undefined2 *)&DAT_451b_28e2)[iVar6] = *(undefined2 *)(iVar6 * 2 + 0x6840);
          *(undefined2 *)(iVar6 * 2 + 0x6840) = uVar5;
          iVar6 = iVar6 + -1;
        }
      }
    }
  }
  DAT_451b_28d2 = 0;
  for (iVar4 = 1; iVar4 < DAT_4125_30f6; iVar4 = iVar4 + 1) {
    if (((undefined2 *)&DAT_451b_28f2)[iVar4] == ((undefined2 *)&DAT_451b_28f0)[iVar4]) {
      uVar5 = 0xffff;
    }
    else {
      uVar5 = 0;
    }
    ((undefined2 *)&DAT_451b_28d2)[iVar4] = uVar5;
  }
  bVar3 = false;
  DAT_451b_28d0 = 0;
  while( true ) {
    if (DAT_4125_30f6 <= DAT_451b_28d0) goto LAB_89e0_19ea;
    if (((undefined2 *)&DAT_451b_28e2)[DAT_451b_28d0] != 0) break;
    DAT_451b_28d0 = DAT_451b_28d0 + 1;
  }
  bVar3 = true;
  for (iVar4 = 0; iVar4 < DAT_4125_30f6; iVar4 = iVar4 + 1) {
    if (((((undefined2 *)&DAT_451b_28e2)[iVar4] == 0) ||
        (((undefined2 *)&DAT_451b_28f2)[iVar4] != ((undefined2 *)&DAT_451b_28f2)[DAT_451b_28d0])) &&
       ((((undefined2 *)&DAT_451b_28e2)[iVar4] != 0 ||
        (((undefined2 *)&DAT_451b_28f2)[iVar4] == ((undefined2 *)&DAT_451b_28f2)[DAT_451b_28d0]))))
    {
      bVar3 = false;
    }
  }
LAB_89e0_19ea:
  if (bVar3) {
    ((undefined2 *)&DAT_451b_28d2)[DAT_451b_28d0] = 1;
  }
  else {
    DAT_451b_28d0 = -1;
  }
  return;
}


