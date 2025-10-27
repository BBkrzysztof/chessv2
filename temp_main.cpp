// #include "Parser/Parser.cpp"
// #include "Utils.cpp"
//
// using namespace std;
//
// int main() {
//     string fen = "rnb1kbnr/ppp2ppp/8/3pp3/4P3/P7/1PPPqPPP/RNBQK1NR w KQkq c6 0 1";
//     string fen2 = "rnb1kbnr/1pp2ppp/1P6/p2pp3/4P3/P7/2PPqPPP/RNBQK1NR w KQkq - 0 1";
//     string fen3 = "r1bqkb1r/pppp1ppp/2n5/8/2Bpn3/2P2N2/PP3PPP/RNBQK2R w KQkq - 0 6";
//     auto parser = new Parser(fen3);
//     auto container = parser->parseFen();
//
//     auto encodedBoard = Parser::encodeBoard(container.get());
//     auto parsedFen = Parser::encodeGameState(encodedBoard, container.get());
//
//     Utils::displayBoard(container.get());
//
//     return 0;
// }
//
// using U64 = uint64_t;
//
// static void print_bb(U64 b){
//     for(int r=7; r>=0; --r){
//         for(int f=0; f<8; ++f){
//             int sq=r*8+f;
//             std::cout<<( (b>>sq)&1ULL ? " X" : " ." );
//         }
//         std::cout<<"\n";
//     }
// }
//
// // --- MASKI (bez pól brzegowych na końcach promieni) ---
// U64 rook_mask(int sq){
//     U64 m=0; int f=file_of(sq), r=rank_of(sq);
//     const int dr[4]={+1,-1,0,0}, df[4]={0,0,+1,-1};
//     for(int d=0; d<4; ++d){
//         int rr=r, ff=f;
//         while(true){
//             rr+=dr[d]; ff+=df[d];
//             if(rr<0||rr>7||ff<0||ff>7) break;
//             int s=rr*8+ff; m|=bit(s);
//             int rr2=rr+dr[d], ff2=ff+df[d];
//             if(rr2<0||rr2>7||ff2<0||ff2>7){ m&=~bit(s); break; }
//         }
//     }
//     return m;
// }
//
// U64 sliding_attacks_rook(int sq, U64 occSubset){
//     U64 a=0; int f=file_of(sq), r=rank_of(sq);
//     const int dr[4]={+1,-1,0,0}, df[4]={0,0,+1,-1};
//     for(int d=0; d<4; ++d){
//         int rr=r, ff=f;
//         while(true){
//             rr+=dr[d]; ff+=df[d];
//             if(rr<0||rr>7||ff<0||ff>7) break;
//             int s=rr*8+ff; a|=bit(s);
//             if(occSubset & bit(s)) break; // zatrzymaj się na pierwszym blokerze (capturable)
//         }
//     }
//     return a;
// }
//
// // --- PREKOMPUTACJA dla jednego pola (carry-rippler) ---
// void precompute_rook_square(int sq, U64 mask, bool usePEXT, U64 magic, std::vector<U64>& table){
//     int k = popcount64(mask);
//     table.assign(1u<<k, 0ULL);
//     for(U64 subset = mask;; subset = (subset - 1) & mask){
//         uint64_t idx = usePEXT ? (uint64_t)_pext_u64(subset, mask)
//                                : (uint64_t)((subset * magic) >> (64 - k));
//         table[idx] = sliding_attacks_rook(sq, subset);
//         if(subset==0) break;
//     }
// }
//
// int main(){
//     // --- 1) PRZYKŁADOWA POZYCJA ---
//     // Indeksowanie: a1=0 ... h8=63; d4 = file=3, rank=3 -> 3 + 3*8 = 27
//     const int sqR = 27; // biała wieża na d4
//     U64 white = 0, black = 0;
//
//     // Blokery: ustawmy po jednym w każdą stronę od d4:
//     // d6 (d-file, rank 5) -> 3 + 5*8 = 43 (czarny)
//     // f4 (file 5, rank 3) -> 29 (czarny)
//     // d2 (file 3, rank 1) -> 11 (czarny)
//     // b4 (file 1, rank 3) -> 25 (biały, żeby pokazać odfiltrowanie własnych)
//     black |= bit(59) | bit(29) | bit(11);
//     white |= bit(25) | bit(sqR); // nasza wieża + biały bloker na b4
//     U64 occAll = white | black;
//
//     // --- 2) PREKOMPUTACJA TABLICY dla pola d4 ---
//     const bool USE_PEXT = true; // jeśli nie masz BMI2, ustaw na false i podaj sensowny magic
//     U64 mask = rook_mask(sqR);
//     std::vector<U64> table;
//     U64 magic = 0xA180022080400230ULL; // przykładowy; używany tylko jeśli USE_PEXT=false
//     precompute_rook_square(sqR, mask, USE_PEXT, magic, table);
//
//     // --- 3) RUNTIME: relevant, indeks, lookup ---
//     U64 relevant = occAll & mask;
//
//     int k = popcount64(mask);
//     uint64_t idx = USE_PEXT ? (uint64_t)_pext_u64(relevant, mask)
//                             : (uint64_t)((relevant * magic) >> (64 - k));
//      U64 attacksAll = table[idx];
//
//     // Usuwamy własne pola (nie możemy bić swoich bierek)
//     U64 legalPseudo = attacksAll & ~white;
//
//     // --- 4) WIZUALIZACJA ---
//     std::cout << "Mask (relevant squares for rook @ d4):\n";
//     print_bb(mask);
//     std::cout << "\nOccupancy (all pieces):\n";
//     print_bb(occAll);
//     std::cout << "\nRook attacks from d4 (pseudo-legal, after filtering own pieces):\n";
//     print_bb(legalPseudo);
//
//     // Dla ciekawskich: heksy
//     std::cout << std::hex << "\nmask=0x" << mask
//               << " relevant=0x" << relevant
//               << " idx=0x" << idx
//               << " attacks=0x" << attacksAll
//               << " legal=0x" << legalPseudo << std::dec << "\n";
// }
//
//
