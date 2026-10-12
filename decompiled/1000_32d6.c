
void __cdecl16far free(void *p)

{
  DAT_1000_319c = 0x4125;
  if (p._2_2_ != 0) {
    if (p._2_2_ == DAT_1000_3198) {
      __heap_release_last();
    }
    else {
      __heap_join_free();
    }
  }
  return;
}


