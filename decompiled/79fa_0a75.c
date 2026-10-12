
void __cdecl16far setup_city_production(void)

{
  int iVar1;
  char cVar2;
  int iVar3;
  int iVar4;
  undefined1 local_42 [22];
  undefined1 local_2c;
  undefined1 local_2a;
  undefined1 local_28;
  undefined1 local_26;
  int local_4;
  
  for (iVar3 = 0; iVar3 < city_count; iVar3 = iVar3 + 1) {
    for (iVar4 = 0; iVar4 < 4; iVar4 = iVar4 + 1) {
      if (*(char *)(iVar3 * 0x41 + iVar4 + 0x1593) == '\x05') {
        *(undefined1 *)(iVar3 * 0x41 + iVar4 + 0x1593) = 0xff;
      }
    }
    for (iVar4 = 0; iVar4 < 4; iVar4 = iVar4 + 1) {
      iVar1 = iVar4;
      if (*(char *)(iVar3 * 0x41 + iVar4 + 0x1593) == -1) {
        for (; iVar1 < 3; iVar1 = iVar1 + 1) {
          *(undefined1 *)(iVar3 * 0x41 + iVar1 + 0x1593) =
               *(undefined1 *)(iVar3 * 0x41 + iVar1 + 0x1594);
        }
        *(undefined1 *)(iVar3 * 0x41 + 0x1596) = 0xff;
      }
    }
  }
  for (local_4 = 0; local_4 < 0x1d; local_4 = local_4 + 1) {
    armytype_lookup(local_4,local_42);
    for (iVar3 = 0; iVar3 < city_count; iVar3 = iVar3 + 1) {
      for (iVar4 = 0; iVar4 < 4; iVar4 = iVar4 + 1) {
        if (*(char *)(iVar3 * 0x41 + iVar4 + 0x1593) == local_4) {
          *(undefined1 *)(iVar3 * 0x41 + iVar4 + 0x159b) = local_2c;
          *(undefined1 *)(iVar3 * 0x41 + iVar4 + 0x1597) = local_2a;
          *(undefined1 *)(iVar3 * 0x41 + iVar4 + 0x159f) = local_26;
          *(undefined1 *)(iVar3 * 0x41 + iVar4 + 0x15a3) = local_28;
        }
      }
    }
  }
  for (iVar3 = 0; iVar3 < city_count; iVar3 = iVar3 + 1) {
    sort_city_production(iVar3);
  }
  iVar3 = 0;
  do {
    if (city_count <= iVar3) {
      return;
    }
    iVar4 = 0;
    while ((iVar4 < 4 && (-1 < *(char *)(iVar3 * 0x41 + iVar4 + 0x1593)))) {
      iVar1 = dice(1,100,0);
      if (iVar1 < 10) {
        iVar1 = dice(1,100,0);
        if (iVar1 < 0x3c) {
          if (*(char *)(iVar3 * 0x41 + iVar4 + 0x159b) + 1 < 9) {
            cVar2 = *(char *)(iVar3 * 0x41 + iVar4 + 0x159b) + '\x01';
          }
          else {
            cVar2 = '\t';
          }
        }
        else if (*(char *)(iVar3 * 0x41 + iVar4 + 0x159b) + -1 < 2) {
          cVar2 = '\x01';
        }
        else {
          cVar2 = *(char *)(iVar3 * 0x41 + iVar4 + 0x159b) + -1;
        }
        *(char *)(iVar3 * 0x41 + iVar4 + 0x159b) = cVar2;
      }
      iVar1 = dice(1,100,0);
      if (iVar1 < 0x14) {
        iVar1 = dice(1,100,0);
        if (iVar1 < 10) {
          cVar2 = *(char *)(iVar3 * 0x41 + iVar4 + 0x159f) + '\x04';
        }
        else {
          if (0x3b < iVar1) {
            if (iVar1 < 0x5f) {
              if (*(char *)(iVar3 * 0x41 + iVar4 + 0x159f) + -2 < 3) {
LAB_79fa_0d5f:
                cVar2 = '\x02';
              }
              else {
                cVar2 = *(char *)(iVar3 * 0x41 + iVar4 + 0x159f) + -2;
              }
            }
            else {
              if (*(char *)(iVar3 * 0x41 + iVar4 + 0x159f) + -4 < 3) goto LAB_79fa_0d5f;
              cVar2 = *(char *)(iVar3 * 0x41 + iVar4 + 0x159f) + -4;
            }
            *(char *)(iVar3 * 0x41 + iVar4 + 0x159f) = cVar2;
            goto LAB_79fa_0d72;
          }
          cVar2 = *(char *)(iVar3 * 0x41 + iVar4 + 0x159f) + '\x02';
        }
        *(char *)(iVar3 * 0x41 + iVar4 + 0x159f) = cVar2;
      }
LAB_79fa_0d72:
      if (*(char *)(iVar3 * 0x41 + iVar4 + 0x159f) < '\x06') {
        *(undefined1 *)(iVar3 * 0x41 + iVar4 + 0x159f) = 6;
      }
      iVar1 = dice(1,100,0);
      if (iVar1 < 10) {
        iVar1 = dice(1,100,0);
        if (iVar1 < 0x3c) {
          cVar2 = *(char *)(iVar3 * 0x41 + iVar4 + 0x15a3) -
                  *(char *)(iVar3 * 0x41 + iVar4 + 0x15a3) / '\x04';
        }
        else {
          cVar2 = *(char *)(iVar3 * 0x41 + iVar4 + 0x15a3) +
                  *(char *)(iVar3 * 0x41 + iVar4 + 0x15a3) / '\x04';
        }
        *(char *)(iVar3 * 0x41 + iVar4 + 0x15a3) = cVar2;
      }
      iVar1 = dice(1,100,0);
      if (iVar1 < 10) {
        iVar1 = dice(1,100,0);
        if (iVar1 < 0x3c) {
          if (*(char *)(iVar3 * 0x41 + iVar4 + 0x1597) + -1 < 2) {
            cVar2 = '\x01';
          }
          else {
            cVar2 = *(char *)(iVar3 * 0x41 + iVar4 + 0x1597) + -1;
          }
          *(char *)(iVar3 * 0x41 + iVar4 + 0x1597) = cVar2;
        }
        else {
          *(char *)(iVar3 * 0x41 + iVar4 + 0x1597) =
               *(char *)(iVar3 * 0x41 + iVar4 + 0x1597) + '\x01';
        }
      }
      iVar4 = iVar4 + 1;
    }
    iVar3 = iVar3 + 1;
  } while( true );
}


