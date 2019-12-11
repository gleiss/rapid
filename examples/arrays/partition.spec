func main()
{
  const Int[] a;
  const Int alength;

  Int[] b;
  Int blength = 0;
  Int[] c;
  Int clength = 0;

	Int i = 0;
	while(i < alength)
	{
    if(a[i] >= 0)
    {
      b[blength] = a[i];
      blength = blength + 1;
    }
    else
    {
      c[clength] = a[i];
      clength = clength + 1;
    }
    i = i + 1;
	}
}

(conjecture
   (forall ((pos Int))
      (=>
        (and
          (<= 0 alength)
          (<= 0 pos)
          (< pos (blength main_end))
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
          (< pos (clength main_end))
        )
        (< (c main_end pos) 0)
      )
   )
)

(conjecture
  (forall ((pos Int))
    (=>
      (and
        (<= 0 alength)
        (<= 0 pos)
        (< pos alength)
        (<= 0 (a pos))
      )
      (exists ((pos2 Int))
        (and
          (<= 0 pos2)
          (< pos2 (blength main_end))
          (= (b main_end pos) (a pos))
        )
      )
    )
  )
)

(conjecture
  (forall ((pos Int))
    (=>
      (and
        (<= 0 alength)
        (<= 0 pos)
        (< pos alength)
        (< (a pos) 0)
      )
      (exists ((pos2 Int))
        (and
          (<= 0 pos2)
          (< pos2 (clength main_end))
          (= (c main_end pos) (a pos))
        )
      )
    )
  )
)

