# What is this code?

This is an implementation of the Re-Pair algorithm for research purposes. Try running it on different data :)! Still a work in progress.

# Class Diagram


```mermaid
classDiagram
    direction LR
    
    class Repair {
        -QUEUE q
        -unordered_map~pair, PAIR*~ ht
        -TSEQ seq
        -st rule
        -vector~st~ ruleHistory
        -firstPass()
        -compress(bool)
        +run(bool)
        +output()
    }

    class QUEUE {
        -vector~PAIRNODE*~ buckets
        +addPair(PAIR*)
        +removePair(PAIR*)
    }

    class PAIRNODE {
        +PAIR* p
        +PAIRNODE* next
        +PAIRNODE* prev
    }

    class TSEQ {
        <<THREADED SEQUENCE>>
        -vector~SEQ~ seq
        +next(st)
        +prev(st)
        +operator[](st)
    }

    class PAIR {
        +st left
        +st right
        +st freq
        +st f_pos
        +st b_pos
        +PAIR* next
        +PAIR* prev
        +PAIRNODE* node
    }

    class SEQ {
        +st code
        +st prev
        +st next
    }

    %% Relationships
    Repair *-- QUEUE : owns
    Repair *-- TSEQ: owns
    QUEUE o-- PAIRNODE : contains buckets (linked lists) of
    PAIRNODE o-- PAIR : points to data
    PAIR --> QUEUE : can be added to bucket
    TSEQ *-- SEQ : composed of
```



(This is all for now, I'll add more stuff later)
