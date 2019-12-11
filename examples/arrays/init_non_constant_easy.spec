// Variation of init_non_constant, where multiplication is removed.
// Properties:
// 1) after the execution, a[pos] has the value pos+v for each position pos in the array
// 2) if v is greater than 0, a[pos] is greater than pos
// 3) if v is strictly greater than 0, a[pos] is strictly greater than pos (variation of property 2)
// 4) if v is some positive constant, a[pos] is strictly greater than pos (variation of property 2)

func main()
{
	Int[] a;
	const Int alength;
	const Int v;

	Int i = 0;
	while(i < alength)
	{
		a[i] = i + v;
		i = i + 1;
	}
}

(conjecture
   (forall ((pos Int))
      (=>
         (and
            (<= 0 alength)
            (<= 0 pos)
            (< pos alength)
         )
         (= (a main_end pos) (+ pos v))
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
            (<= 0 v)
         )
         (<= pos (a main_end pos))
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
            (< 0 v)
         )
         (< pos (a main_end pos))
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
            (= v 80)
         )
         (< pos (a main_end pos))
      )
   )
)