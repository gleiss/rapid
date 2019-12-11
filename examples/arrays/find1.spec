// The find example from the Invgen-repo where we want to prove that
// 1) if we find a position, the element at that position is equal to v
// 2) variation of property 1)
// 3) variation of property 1)
// 4) variation of property 1)
// 5) forall positions pos between 0 and r, A[pos] is different from v

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
		(and
			(<= 0 alength)
			(not (= (r main_end) alength))
		)
		(= (a (r main_end)) v)
	)
)

(conjecture
	(=>
		(and
			(<= 0 alength)
			(< (r main_end) alength)
		)
		(= (a (r main_end)) v)
	)
)

(conjecture
	(=>
		(and
			(<= 0 alength)
			(not (= (r main_end) alength))
		)
		(exists ((pos Int))
			(= (a pos) v)
		)
	)
)

(conjecture
	(=>
		(and
			(<= 0 alength)
			(< (r main_end) alength)
		)
		(exists ((pos Int))
			(= (a pos) v)
		)
	)
)

(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 alength)
				(<= 0 pos)
				(< pos (r main_end))
			)
			(not (= (a pos) v))
		)
	)
)
