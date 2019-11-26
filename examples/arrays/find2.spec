// a new encoding of find, which is easier for the solvers, where we want to prove that
// 1) if we found a position p, then we have A[p] == v.
// 2) if we found a position, then for all previous positions p, we have A[p] != v.
// 3) if we found no position, then forall positions p we have that A[i]= v.

func main()
{
	const Int[] a;
	const Int alength;
	const Int v;

	Int i = 0;
	while (i < alength && a[i] != v)
	{
		i = i + 1;
	}
}

(conjecture
	(=>
		(<= 0 alength)
		(or
			(= (i main_end) alength)
			(= (a (i main_end)) v)
		)
	)
)

(conjecture
	(=>
		(and
			(<= 0 alength)
			(< (i main_end) alength)
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
				(< pos (i main_end))
			)
			(not (= (a pos) v))
		)
	)
)

// This conjecture is unnecessary as this case is already covered by
// the conjecture above.
(conjecture
	(forall ((pos Int))
		(=>
			(and
				(<= 0 alength)
				(= (i main_end) alength)
				(<= 0 pos)
				(< pos alength)
			)
			(not (= (a pos) v))
		)
	)
)
