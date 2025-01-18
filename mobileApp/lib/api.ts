import { onValue, ref } from "firebase/database";
import { db } from "./firebase/firebase";

export function getInventory(){
    const inventoryRef = ref(db,'Inventory/stock');
    onValue(inventoryRef,(snapshot) => {
    const data = snapshot.val();
    console.log(data);
});
}
