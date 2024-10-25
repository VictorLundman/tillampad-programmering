# ClockProject
 
Projekt gjort för kursen tillämpad programmering. 

## Extra function

Den extra funcitonen är ett Flappy Bird spel, och därför är den fysiska komponenten en knapp för att styra fågeln. Den använder inte några avancerade fysiska komponenter, men programmeringen var lite svårare. Spelet inehåller en fågel, två hinder som kan vara på skärmen samtidigt. Den räknar också poäng. När man krockar kommerman tillbaka till skärment med tiden, där man också kan se sina poäng. 

## Problem

* Knappen fungerade inte, den pendlare mellan på och av utan att den trycktes ned. Jag förstod att jag hade kopplat fel, då det har hänt mig tidigare och då var problemet samma.  Lösningen var att koppla om knappen på ett korrekt sätt. 
* Skärmen visade inget. Problemet var att jag använt en return istället för en continue när standardskärmen (inte spelet) skrevs ut för att skippa att rita ut spelet. Det tog en stund att debugga, men till slut kunde jag lösa problemet genom att läsa igenom koden till oledWrite functionen. 
* Att upptäcka om det var en kollision mellan fågeln och hindren var svårt. I mitt första försök fick jag matten fel då jag tänkte att y-värdet 0 var längst ned på skärmen när det i själva verkat var högst upp. Det fixade jag genom att skriva om checken för kollision, och då funkade det. 