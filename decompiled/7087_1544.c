
void __cdecl16far sort_city_production(int city)

{
  undefined1 uVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  int iVar5;
  undefined2 unaff_SS;
  undefined1 local_54 [30];
  uint local_36;
  int local_14 [8];
  int local_4;
  
  if (-1 < *(char *)(city * 0x41 + 0x1593)) {
    for (iVar4 = 0;
        (iVar4 < 4 &&
        (local_14[iVar4] = (int)*(char *)(city * 0x41 + iVar4 + 0x1593), -1 < local_14[iVar4]));
        iVar4 = iVar4 + 1) {
      armytype_lookup(local_14[iVar4],local_54);
      local_14[iVar4 + 4] = (local_36 ^ (int)local_36 >> 0xf) - ((int)local_36 >> 0xf);
    }
    for (iVar5 = 1; iVar3 = iVar5, iVar5 < iVar4; iVar5 = iVar5 + 1) {
      while ((0 < iVar3 && (local_14[iVar3 + 4] < local_14[iVar3 + 3]))) {
        local_4 = local_14[iVar3 + 4];
        local_14[iVar3 + 4] = local_14[iVar3 + 3];
        local_14[iVar3 + 3] = local_4;
        uVar1 = *(undefined1 *)(city * 0x41 + iVar3 + 0x1593);
        *(undefined1 *)(city * 0x41 + iVar3 + 0x1593) =
             *(undefined1 *)(city * 0x41 + iVar3 + 0x1592);
        *(undefined1 *)(city * 0x41 + iVar3 + 0x1592) = uVar1;
        uVar1 = *(undefined1 *)(city * 0x41 + iVar3 + 0x159b);
        *(undefined1 *)(city * 0x41 + iVar3 + 0x159b) =
             *(undefined1 *)(city * 0x41 + iVar3 + 0x159a);
        *(undefined1 *)(city * 0x41 + iVar3 + 0x159a) = uVar1;
        uVar1 = *(undefined1 *)(city * 0x41 + iVar3 + 0x15a3);
        *(undefined1 *)(city * 0x41 + iVar3 + 0x15a3) =
             *(undefined1 *)(city * 0x41 + iVar3 + 0x15a2);
        *(undefined1 *)(city * 0x41 + iVar3 + 0x15a2) = uVar1;
        uVar1 = *(undefined1 *)(city * 0x41 + iVar3 + 0x159f);
        *(undefined1 *)(city * 0x41 + iVar3 + 0x159f) =
             *(undefined1 *)(city * 0x41 + iVar3 + 0x159e);
        *(undefined1 *)(city * 0x41 + iVar3 + 0x159e) = uVar1;
        cVar2 = *(char *)(city * 0x41 + iVar3 + 0x1597);
        local_4 = (int)cVar2;
        *(undefined1 *)(city * 0x41 + iVar3 + 0x1597) =
             *(undefined1 *)(city * 0x41 + iVar3 + 0x1596);
        *(char *)(city * 0x41 + iVar3 + 0x1596) = cVar2;
        iVar3 = iVar3 + -1;
      }
    }
  }
  return;
}


