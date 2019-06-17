// The find example from the Invgen-repo where we want to prove that
// 1) if we find a position, the element at that position is equal to v
// 2) forall positions pos between 0 and r, A[pos] is different from v
// 3) if we didn't find any position, then at all positions pos, A[pos] is different from v

func main()
{
	const Int[] a;
	const Int alength;
	const Int v;

	Int i = 0;
	Int r = alength;

	while (i < alength && r == alength)
	{
		if (a[i] == v)
		{
			r = i;
		}
		else
		{
			skip;
		}
		i = i + 1;
	}
}

(conjecture
	(=>
		(<= 0 alength)
		(or
			(= (r main_end) alength)
			(= (a (r main_end)) v)
		)
	)
)

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 pos)
				(< pos (r main_end))
			)
			(not (= (a pos) v))
		)
	)
)

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(= (r main_end) alength)
				(<= 0 pos)
				(< pos alength)
			)
			(not (= (a pos) v))
		)
	)
)