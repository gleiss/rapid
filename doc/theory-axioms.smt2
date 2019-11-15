
; ================================================
; Part 1: Pure Term Algebra axioms for Nat 
; (added internally by Vampire (master branch) and therefore commented out)
; ================================================
(declare-datatypes ((Nat 0)) (( (zero) (s (p Nat)) )) )

;; exhaustiveness
;(assert
;	(forall ((it1 Nat))
;		(or
;			(= it1 zero)
;			(exists ((it2 Nat))
;				(= it1 (s it2))
;			)
;		)
;	)
;)
;
;; distinctness
;(assert
;	(forall ((it Nat))
;		(not
;			(= zero (s it))
;		)
;	)
;)
;
;; injectivity of s
;(assert
;	(forall ((it1 Nat) (it2 Nat))
;		(=>
;			(= (s it1) (s it2))
;			(= it1 it2)
;		)
;	)
;)

; ================================================
; Part 2: Extension of Term Algebra with Sub predicate (where (Sub t1 t2) denotes that t1 is strictly smaller than t2)
; ================================================
(declare-fun Sub (Nat Nat) Bool)

; zero is smaller than the successor of any number
(assert
	(forall ((it Nat))
		(Sub zero (s it))
	)
)

; encoding (<= it1 it2) as (or (= it1 it2) (< it1 it2))
(assert
	(forall ((it1 Nat) (it2 Nat))
		(=
			(Sub it1 (s it2))
			(or
				(= it1 it2)
				(Sub it1 it2)
			)
		)
	)
)

; a somewhat dual version of the previous axiom
(assert
	(forall ((it1 Nat) (it2 Nat))
		(=
			(not
				(Sub (s it1) it2)
			)
			(or
				(= (s it1) it2)
				(not
					(Sub it1 it2)
				)
			)
		)
	)
)

; monotonicity of s with respect to <
(assert
	(forall ((it1 Nat) (it2 Nat))
		(=
			(Sub it1 it2)
			(Sub (s it1) (s it2))
		)
	)
)

; ------------------------------------------
; We need multiple versions of transitivity,
; in order to support all combinations of strict
; and non-strict inequalities
; ------------------------------------------


; transitivity <,<
(assert
	(forall ((it1 Nat) (it2 Nat) (it3 Nat))
		(=>
			(and
				(Sub it1 it2)
				(Sub it2 it3)
			)
			(Sub it1 it3)
		)
	)
)

; transitivity <=,<
(assert
	(forall ((it1 Nat) (it2 Nat) (it3 Nat))
		(=>
			(and
				(Sub it1 (s it2))
				(Sub it2 it3)
			)
			(Sub it1 it3)
		)
	)
)

; transitivity <,<=
(assert
	(forall ((it1 Nat) (it2 Nat) (it3 Nat))
		(=>
			(and
				(Sub it1 it2)
				(Sub it2 (s it3))
			)
			(Sub it1 it3)
		)
	)
)

; transitivity <=,<=
(assert
	(forall ((it1 Nat) (it2 Nat) (it3 Nat))
		(=>
			(and
				(Sub it1 (s it2))
				(Sub it2 (s it3))
			)
			(Sub it1 (s it3))
		)
	)
)

; ------------------------------------------
; The following axioms (totality and disjointness) 
; formalize the fact that for two natural numbers it1 and it2, 
; always EXACTLY one of the following facts holds: 
; it1<it2
; it1=it2
; it1>it2
; ------------------------------------------

; totality
(assert
	(forall ((it1 Nat) (it2 Nat))
		(or
			(Sub it1 it2)
			(= it1 it2)
			(Sub it2 it1)
		)
	)
)

; disjointness 1
(assert
	(forall ((it1 Nat) (it2 Nat))
		(=>
			(Sub it1 it2)
			(and
				(not
					(= it1 it2)
				)
				(not
					(Sub it2 it1)
				)
			)
		)
	)
)

; disjointness 2
; note that this axiom is equivalent to it1 !< it1
(assert
	(forall ((it1 Nat) (it2 Nat))
		(=>
			(= it1 it2)
			(not 
				(Sub it1 it2)
			)
		)
	)
)

; ================================================
; Part 3: Integer axioms 
; (added internally by Vampire (master branch) and therefore commented out)
; ================================================
; TODO: Rewrite these axioms in smtlib-syntax instead of TPTP-Vampire-output
; Theory clause: 3. ! [X1 : $int,X0 : $int] : $sum(X0,X1) = $sum(X1,X0) [theory axiom]
; Theory clause: 5. ! [X1 : $int,X0 : $int,X2 : $int] : $sum(X0,$sum(X1,X2)) = $sum($sum(X0,X1),X2) [theory axiom]
; Theory clause: 7. ! [X0 : $int] : $sum(X0,0) = X0 [theory axiom]
; Theory clause: 9. ! [X1 : $int,X0 : $int] : $uminus($sum(X0,X1)) = $sum($uminus(X1),$uminus(X0)) [theory axiom]
; Theory clause: 11. ! [X0 : $int] : 0 = $sum(X0,$uminus(X0)) [theory axiom]
; Theory clause: 13. ! [X0 : $int] : ~$less(X0,X0) [theory axiom]
; Theory clause: 15. ! [X1 : $int,X0 : $int,X2 : $int] : (~$less(X0,X1) | ~$less(X1,X2) | $less(X0,X2)) [theory axiom]
; Theory clause: 17. ! [X1 : $int,X0 : $int] : ($less(X0,X1) | $less(X1,X0) | X0 = X1) [theory axiom]
; Theory clause: 19. ! [X1 : $int,X0 : $int,X2 : $int] : (~$less(X0,X1) | $less($sum(X0,X2),$sum(X1,X2))) [theory axiom]
; Theory clause: 21. ! [X1 : $int,X0 : $int] : ($less(X0,X1) | $less(X1,$sum(X0,1))) [theory axiom]
; Theory clause: 23. ! [X0 : $int] : $uminus($uminus(X0)) = X0 [theory axiom]
; Theory clause: 25. ! [X1 : $int,X0 : $int] : (~$less(X0,X1) | ~$less(X1,$sum(X0,1))) [theory axiom]