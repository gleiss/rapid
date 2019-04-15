(two-traces)
func main()
{
	Int h;
	Int l;

	h = l + 4;
	l = l - 5;
}

(conjecture
	(=>
		(= (l l7 t1) (l l7 t2))	
		(= (l main_end t1) (l main_end t2))
	)
)