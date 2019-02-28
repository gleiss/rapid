func main()
{
  Int[] bb;
  Int[] cc;
  const Int[] aa;
  const Int alength;

	Int a = 0;
  Int b = 0;
  Int c = 0;
	while(a < alength)
	{
    if(aa[a] >= 0)
    {
      bb[b] = aa[a];
      b = b + 1;
    }
    else
    {
      cc[c] = aa[a];
      c = c + 1;
    }
    a = a + 1;
	}
}

(assert-not
   (forall ((pos Int))
      (=>
        (and
          (<= 0 pos)
          (< pos (b main_end))
          (<= 0 alength)
        )
        (<= 0 (bb main_end pos))
      )
   )
)
