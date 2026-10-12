
void FUN_4fef_0a4b(void)

{
  byte *pbVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  byte *pbVar5;
  int iVar6;
  int iVar7;
  undefined2 uVar8;
  undefined2 uStack_a;
  undefined2 uStack_6;
  
  for (iVar7 = 0; iVar7 < 0x9c; iVar7 = iVar7 + 1) {
    for (iVar6 = 0; iVar6 < 0x70; iVar6 = iVar6 + 1) {
      if ((*(byte *)(iVar7 * 0x70 + iVar6) & 0x1f) != 0) {
        iVar3 = FUN_4fef_0ee3();
        if ((iVar6 + iVar7 == 0) || ((iVar6 + iVar7) / 2 != (iVar6 + iVar7 + -1) / 2)) {
          iVar4 = 1;
        }
        else {
          iVar4 = 0;
        }
        uVar8 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
        if (*(char *)((int)DAT_451b_002a + iVar3 + 0x364) < '\0') {
          iVar3 = dice(1,10,0);
          uVar8 = (undefined2)((ulong)DAT_451b_002a >> 0x10);
          if (iVar3 < 6) {
            uVar8 = *(undefined2 *)((int)DAT_451b_002a + iVar4 * 2 + 0x624);
            pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
            *pbVar5 = 0;
            pbVar1 = pbVar5;
            *pbVar1 = *pbVar1 | (byte)uVar8;
            *(undefined1 *)((int)DAT_451b_002a + iVar7 * 0x70 + iVar6 + 0x6120) = 7;
          }
          else {
            uVar8 = *(undefined2 *)((int)DAT_451b_002a + iVar4 * 2 + 0x5d8);
            pbVar5 = (byte *)(iVar7 * 0xe0 + iVar6 * 2);
            *pbVar5 = 0;
            pbVar1 = pbVar5;
            *pbVar1 = *pbVar1 | (byte)uVar8;
          }
        }
        else {
          cVar2 = *(char *)((int)DAT_451b_002a + iVar3 + 0x364);
          pbVar5 = (byte *)(iVar7 * 0x70 + iVar6);
          pbVar1 = pbVar5;
          *pbVar1 = *pbVar1 & 0xe0;
          pbVar1 = pbVar5;
          *pbVar1 = *pbVar1 | cVar2 + 1U & 0x1f;
        }
      }
    }
  }
  iVar7 = dice(1,10,10);
  for (uStack_6 = 0; uStack_6 < iVar7; uStack_6 = uStack_6 + 1) {
    for (uStack_a = 0; uStack_a < 10000; uStack_a = uStack_a + 1) {
      iVar6 = dice(1,0x70,-1);
      iVar3 = dice(1,0x9c,-1);
      if ((*(byte *)(iVar3 * 0x70 + iVar6) & 0x1f) == 2) {
        pbVar5 = (byte *)(iVar3 * 0x70 + iVar6);
        pbVar1 = pbVar5;
        *pbVar1 = *pbVar1 & 0xe0;
        pbVar1 = pbVar5;
        *pbVar1 = *pbVar1 | 0x11;
        break;
      }
    }
  }
  iVar7 = dice(1,10,10);
  uStack_6 = 0;
  do {
    if (iVar7 <= uStack_6) {
      return;
    }
    for (uStack_a = 0; uStack_a < 10000; uStack_a = uStack_a + 1) {
      iVar6 = dice(1,0x70,-1);
      iVar3 = dice(1,0x9c,-1);
      if ((*(byte *)(iVar3 * 0x70 + iVar6) & 0x1f) == 1) {
        pbVar5 = (byte *)(iVar3 * 0x70 + iVar6);
        pbVar1 = pbVar5;
        *pbVar1 = *pbVar1 & 0xe0;
        pbVar1 = pbVar5;
        *pbVar1 = *pbVar1 | 0x10;
        break;
      }
    }
    uStack_6 = uStack_6 + 1;
  } while( true );
}


