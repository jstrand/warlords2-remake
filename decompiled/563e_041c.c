
int __cdecl16far ai_assault_revalidate(int param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 unaff_SS;
  undefined2 local_18;
  byte abStack_11 [7];
  uint local_a;
  undefined2 local_8;
  int local_6;
  int local_4;
  
  local_4 = 0;
  uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
  local_6 = *(int *)((int)DAT_451b_018e + param_1 * 0x5c + 0x24e);
  local_8 = *(undefined2 *)((int)DAT_451b_018e + param_1 * 0x5c + 0x250);
  for (iVar3 = 0; iVar3 < 6; iVar3 = iVar3 + 1) {
    uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
    iVar6 = (int)DAT_451b_018e;
    iVar7 = *(int *)(iVar6 + param_1 * 0x5c + iVar3 * 2 + 0x25a);
    if (iVar7 != -1) {
      if ((*(int *)(iVar6 + 0x46) == iVar7) || (*(char *)(iVar7 * 0x41 + 0x1592) != local_6)) {
        *(undefined2 *)(iVar6 + param_1 * 0x5c + iVar3 * 2 + 0x25a) = 0xffff;
      }
      else {
        local_4 = local_4 + 1;
      }
    }
  }
  FUN_5db9_0656(local_8,0,abStack_11 + 1);
  iVar3 = 6;
  do {
    do {
      do {
        do {
          iVar7 = iVar3;
          iVar3 = iVar7 + -1;
          if (iVar7 == 0) {
            return local_4;
          }
          local_a = (uint)abStack_11[iVar7];
        } while ((local_a == 0xff) || (*(char *)(local_a * 0x41 + 0x1592) != local_6));
        uVar8 = (undefined2)((ulong)DAT_451b_018e >> 0x10);
        iVar6 = (int)DAT_451b_018e;
      } while ((*(byte *)(iVar6 + local_a + 0x11e) & 1) != 0);
      bVar1 = true;
      iVar2 = 6;
      while (iVar4 = iVar2 + -1, iVar2 != 0) {
        iVar2 = iVar4;
        if (*(uint *)(iVar6 + param_1 * 0x5c + iVar4 * 2 + 0x25a) == local_a) {
          bVar1 = false;
        }
      }
    } while (!bVar1);
    local_4 = local_4 + 1;
    local_18 = 0;
    iVar2 = -1;
    iVar4 = 6;
    while (iVar5 = iVar4 + -1, iVar4 != 0) {
      if (*(int *)(iVar6 + param_1 * 0x5c + iVar5 * 2 + 0x25a) == -1) {
        local_18 = -1;
        iVar2 = iVar5;
        break;
      }
      iVar4 = iVar5;
      if (local_18 < *(int *)(iVar6 + param_1 * 0x5c + iVar5 * 2 + 0x272)) {
        local_18 = *(int *)(iVar6 + param_1 * 0x5c + iVar5 * 2 + 0x272);
        iVar2 = iVar5;
      }
    }
    if ((iVar2 != -1) &&
       ((local_18 == -1 || ((int)(uint)*(byte *)((int)&local_18 + iVar7 + 1) < local_18)))) {
      *(uint *)(iVar6 + param_1 * 0x5c + iVar2 * 2 + 0x25a) = local_a;
      *(uint *)((int)DAT_451b_018e + param_1 * 0x5c + iVar2 * 2 + 0x272) =
           (uint)*(byte *)((int)&local_18 + iVar7 + 1);
    }
  } while( true );
}


