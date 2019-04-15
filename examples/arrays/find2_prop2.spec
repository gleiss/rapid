// a new encoding of find, which is easier for the solvers, where we want to prove that
// if we found a position, then for all previous positions p, we have A[p] != v.

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
