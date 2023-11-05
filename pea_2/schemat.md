1. Start
2. Wczytaj macierz odległości z pliku wejściowego
3. Inicjalizacja zmiennych i struktur danych
4. Utwórz początkowy węzeł z miastem początkowym
5. Inicjalizuj zmienną kosztu trasy jako nieskończoność
6. Inicjalizuj zmienną trasy jako pustą listę
7. Aktualny węzeł = Początkowy węzeł

8. Pętla Główna
9.   Jeśli Aktualny węzeł jest liściem drzewa (wszystkie miasta odwiedzone):
10.     Oblicz koszt trasy i porównaj z aktualnym najlepszym wynikiem
11.     Jeśli nowy koszt jest mniejszy od najlepszego, zaktualizuj najlepszy wynik i trasę
12.   W przeciwnym razie:
13.     Oblicz dolne ograniczenie (Lower Bound) dla Aktualnego węzła
14.     Jeśli dolne ograniczenie przekracza dotychczasowy najlepszy wynik, przejdź do kroku 19 (przycięcie gałęzi)
15.     W przeciwnym razie:
16.       Wybierz kolejny nieodwiedzony węzeł potomny
17.       Aktualny węzeł = Wybrany węzeł potomny
18.       Przejdź do kroku 8 (kontynuuj przeszukiwanie poddrzewa)

19. Koniec
20. Wyświetl optymalną trasę i jej koszt
21. Zakończ działanie programu