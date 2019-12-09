func main()
{
  Int[] b;
  const Int[] c;
  const Int[] a;
  const Int alength;

	Int i = 0;
  Int j = 0;
	while(i < alength)
	{
    if(a[i] == c[i])
    {
      b[j] = i;
      j = j + 1;
    }
    else
    {
      skip;
    }
    i = i + 1;
	}
}

(conjecture
   (forall ((pos Int))
      (=>
        (and
          (<= 0 pos)
          (< pos (j main_end))
          (<= 0 alength)
        )
        (<= 0 (b main_end pos))
      )
   )
)

(conjecture
  (forall ((pos Int))
    (=>
      (and
        (<= 0 alength)
        (<= 0 pos)
        (< pos (j main_end))
      )
      (= (a (b main_end pos)) (c (b main_end pos)))
    )
  )
)
