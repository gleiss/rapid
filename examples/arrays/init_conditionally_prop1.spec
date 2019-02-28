func main()
{
	Int[] a;
  const Int[] b;
  Int[] c;

	const Int length;

	Int i = 0;
  Int j = 0;
	while(i < length)
	{
		if (a[i] == b[i])
    {
      c[j] = i;
      j = j + 1;
    }
    else
    {
      skip;
    }
		i = i + 1;
	}
}

(assert-not
   (forall ((pos Int))
      (=>
         (and
            (<= 0 pos)
            (< pos (j main_end))
            (<= 0 length)
         )
         (<= (c main_end pos) (- (+ pos (i main_end)) (j main_end)))
      )
   )
)
