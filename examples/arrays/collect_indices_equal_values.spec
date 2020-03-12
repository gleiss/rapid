// generate a new array b containing all indices where the arrays a1 and a2 have the same value.
// Properties:
// 1) Each element of b corresponds to a position in a1/a2.
// 2) For each element of b, the elements in a1 and a2 at the corresponding index are the same.
// 3) If the elements in a1 and a2 are the same at position pos, then there exists an element in b with value pos.
// 4) If the elements in a1 and a2 are the same at position pos, then there exists an element in b with value pos, and that element is between 0 and blength.

func main()
{
  const Int[] a1;
  const Int[] a2;
  const Int alength;

  Int[] b;
  Int blength = 0;

	Int i = 0;
	while(i < alength)
	{
    if(a1[i] == a2[i])
    {
      b[blength] = i;
      blength = blength + 1;
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
          (<= 0 alength)
          (<= 0 pos)
          (< pos (blength main_end))
        )
        (and
          (<= 0 (b main_end pos))
          (< (b main_end pos) alength)
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
        (< pos (blength main_end))
      )
      (= (a1 (b main_end pos)) (a2 (b main_end pos)))
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
        (= (a1 pos) (a2 pos))
      )
      (exists ((pos2 Int))
        (and
          (= (b main_end pos2) pos)
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
        (= (a1 pos) (a2 pos))
      )
      (exists ((pos2 Int))
        (and
          (<= 0 pos2)
          (< pos2 (blength main_end))
          (= (b main_end pos2) pos)
        )
      )
    )
  )
)