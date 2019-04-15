// a new encoding of find, which is easier for the solvers, where we want to prove that
// if we found no position, then forall positions p we have that A[i]= v.

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
				(= (i main_end) alength)
				(<= 0 pos)
				(< pos alength)
			)
			(not (= (a pos) v))
		)
	)
)
