// a new encoding of find, which is easier for the solvers, where we want to prove that
// if we found a position p, then we have A[p] == v.

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

(assert-not
	(=>
		(<= 0 alength)
		(exists ((pos Int))
			(=>
				(< (i main_end) alength)
				(= (a pos) v)
			)
		)
	)
)
