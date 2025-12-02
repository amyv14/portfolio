import React, { useState, useEffect, useRef } from "react";
import {
  StyleSheet, Text, View, TouchableOpacity, ScrollView,
  ImageBackground, TextInput, Image, Animated
} from 'react-native';
import { router } from 'expo-router';
import { useCart } from '../cartcontext';
import GlobalStyles from "../../styles/GlobalStyleSheet";
import * as Constants from '../../constants';

// type for each item
type Item = {
  id: number;
  name: string;
  price: number;
  category?: string;
  img_route?: string;
};

export default function Category() {
  // state for food items, filters, and category selection
  const [foodItems, setFoodItems] = useState<Item[]>([]);  
  const [filteredItems, setFilteredItems] = useState<Item[]>([]);  
  const [searchTerm, setSearchTerm] = useState("");  
  const [minPrice, setMinPrice] = useState("");  
  const [maxPrice, setMaxPrice] = useState("");  
  const [selectedCategory, setSelectedCategory] = useState<string | null>(null);  

  // cart state and API URL setup
  const { cart, addToCart } = useCart();  
  const API_URL = Constants.IP_ADDRESS + "items";  

  // toast notification state and animation reference
  const [showToast, setShowToast] = useState(false);  
  const toastY = useRef(new Animated.Value(100)).current;

  // function to trigger toast animation
  const triggerToast = () => {  
    setShowToast(true);  
    Animated.sequence([  
      Animated.timing(toastY, {  
        toValue: 0,  
        duration: 300,  
        useNativeDriver: true,  
      }),  
      Animated.delay(1000),  
      Animated.timing(toastY, {  
        toValue: 100,  
        duration: 300,  
        useNativeDriver: true,  
      }),  
    ]).start(() => {  
      requestAnimationFrame(() => setShowToast(false));  
    });
  };

  useEffect(() => {  
    // fetch items from API
    const fetchItems = async () => {  
      try {  
        const response = await fetch(API_URL);  
        const data = await response.json();  
        setFoodItems(data);  
        setFilteredItems(data);  
      } catch (error) {  
        console.error("Error fetching items:", error);  
      }  
    };  
    fetchItems();  
  }, []);  

  useEffect(() => {  
    // filter items based on search, price, and category
    filterItems();  
  }, [searchTerm, minPrice, maxPrice, selectedCategory, foodItems]);  

  const filterItems = () => {  
    // parsing price filter values
    const min = parseFloat(minPrice);  
    const max = parseFloat(maxPrice);  

    // filtering logic
    const matchesSearch = (item: Item) =>  
      item.name.toLowerCase().includes(searchTerm.toLowerCase());  

    const matchesPrice = (item: Item) => {  
      if (isNaN(min) && isNaN(max)) return true;  
      if (!isNaN(min) && item.price < min) return false;  
      if (!isNaN(max) && item.price > max) return false;  
      return true;  
    };

    const matchesCategory = (item: Item) =>  
      !selectedCategory || item.category === selectedCategory;  

    // updating filtered items
    const results = foodItems.filter(  
      (item) =>  
        matchesSearch(item) && matchesPrice(item) && matchesCategory(item)  
    );  

    setFilteredItems(results);  
  };

  // function to calculate total price in the cart
  const calculateTotal = () =>  
    cart  
      .reduce((total, item) => total + parseFloat(item.price as any), 0)  
      .toFixed(2);  

  const total = parseFloat(calculateTotal());  
  const amountLeftOrOver =  
    total > 12 ? (total - 12).toFixed(2) : (12 - total).toFixed(2);  
  const isOver = total > 12;  

  // getting unique categories
  const allCategories = Array.from(  
    new Set(foodItems.map((item) => item.category).filter(Boolean))  
  );

  return (
    <ImageBackground  
      source={require("../../assets/images/background_white.jpg")}  
      style={styles.background}  
    >
      {/* fixed header with cart and total price */}
      <View style={styles.fixedHeader}>  
        <TouchableOpacity  
          style={styles.cartIconWrapper}  
          onPress={() => router.push("/cart")}  
        >
          <Image  
            source={require("../../assets/images/cart.png")}  
            style={styles.cartIcon}  
          />  
          {cart.length > 0 && (  
            <View style={styles.cartBadge}>  
              <Text style={styles.badgeText}>{cart.length}</Text>  
            </View>  
          )}  
        </TouchableOpacity>

        <View style={styles.totalRow}>  
          <Text style={styles.totalPricePinned}>Total: ${total.toFixed(2)}</Text>  
          <Text style={isOver ? styles.overTextPinned : styles.underTextPinned}>  
            {isOver ? `Over: $${amountLeftOrOver}` : `Left: $${amountLeftOrOver}`}  
          </Text>  
        </View>  
      </View>

      {/* scrollable content */}
      <ScrollView contentContainerStyle={[styles.scrollContainer, { paddingTop: 120 }]}>  
        <View style={styles.container}>  
          <Text style={[GlobalStyles.title, { color: 'black'}]}>  
            Shop  
          </Text>

          {/* search bar */}
          <View style={styles.searchWrapper}>  
            <TextInput  
              style={styles.searchBar}  
              placeholder="Search items..."  
              placeholderTextColor="#888"  
              value={searchTerm}  
              onChangeText={(text) => setSearchTerm(text)}  
            />  
          </View>

          {/* category filter */}
          <ScrollView  
            horizontal  
            showsHorizontalScrollIndicator={false}  
            style={styles.categoryScroll}  
          >
            {allCategories.map((cat, index) => (  
              <TouchableOpacity  
                key={index}  
                style={[  
                  styles.categoryButton,  
                  selectedCategory === cat && styles.categorySelected,  
                ]}  
                onPress={() =>  
                  setSelectedCategory((prev) => (prev === cat ? null : cat))  
                }  
              >  
                <Text style={styles.categoryText}>{cat}</Text>  
              </TouchableOpacity>  
            ))}  
          </ScrollView>

          {/* price filters */}
          <View style={styles.priceFilterBox}>  
            <TextInput  
              style={styles.priceInput}  
              placeholder="Min Price"  
              placeholderTextColor="#888"  
              keyboardType="numeric"  
              value={minPrice}  
              onChangeText={setMinPrice}  
            />  
            <TextInput  
              style={styles.priceInput}  
              placeholder="Max Price"  
              placeholderTextColor="#888"  
              keyboardType="numeric"  
              value={maxPrice}  
              onChangeText={setMaxPrice}  
            />  
            <TouchableOpacity style={styles.filterButton} onPress={filterItems}>  
              <Text style={styles.buttonText}>Show Me Results</Text>  
            </TouchableOpacity>  
          </View>

          {/* click to add items to cart */}
          <Text style={styles.Click}>Click Items to Add to Cart!</Text>

          {/* grid of filtered items */}
          <View style={styles.grid}>  
            {filteredItems.map((item, index) => (  
              <TouchableOpacity  
                key={index}  
                style={styles.itemBox}  
                onPress={() => {  
                  addToCart(item);  
                  triggerToast();  
                }}  
              >  
                {item.img_route && item.img_route.trim() ? (  
                  <Image  
                    source={{  
                      uri: `${Constants.IP_ADDRESS}/${encodeURI(item.img_route.trim())}`,  
                    }}  
                    style={styles.itemImage}  
                    onError={() =>  
                      console.warn(`Could not load image for ${item.name}`)  
                    }  
                  />  
                ) : (  
                  <View style={styles.imagePlaceholder} />  
                )}  
                <Text style={styles.itemText} numberOfLines={2}>  
                  {item.name}  
                </Text>  
                <Text style={styles.itemText}>  
                  ${parseFloat(item.price as any).toFixed(2)}  
                </Text>  
              </TouchableOpacity>  
            ))}  
          </View>  
        </View>  
      </ScrollView>

      {/* toast notification */}
      {showToast && (  
        <Animated.View  
          style={[styles.toast, { transform: [{ translateY: toastY }] }]}  
        >  
          <Text style={styles.toastText}>Added to Cart!</Text>  
        </Animated.View>  
      )}
    </ImageBackground>  
  );
}

const styles = StyleSheet.create({
  background: {
    flex: 1,
    width: "100%",
    height: "100%",
    justifyContent: "center",
    alignItems: "center",
  },
  scrollContainer: {
    flexGrow: 1,
    alignItems: "center",
    paddingBottom: 80,
  },
  container: {
    alignItems: "center",
    marginTop: 10,
  },
  cartIconWrapper: {
    position: "absolute",
    top: 50,
    right: 20,
    zIndex: 10,
  },
  cartIcon: {
    width: 40,
    height: 40,
  },
  cartBadge: {
    position: "absolute",
    top: -6,
    right: -6,
    backgroundColor: "red",
    borderRadius: 10,
    paddingHorizontal: 6,
    paddingVertical: 2,
  },
  badgeText: {
    color: "white",
    fontSize: 12,
    fontWeight: "bold",
  },

  fixedHeader: {
  position: "absolute",
  top: 0,
  width: "100%",
  height: 120,
  backgroundColor: 'rgba(255, 255, 255, 0.90)',
  paddingHorizontal: 20,
  borderBottomColor: "#ccc",
  borderBottomWidth: 1,
  alignItems: "center",
  zIndex: 999,            
  elevation: 10,      
},
totalPricePinned: {
  fontSize: 18,
  fontWeight: "bold",
  color: "black",
  },
underTextPinned: {
  color: "green",
  fontSize: 18,
  fontWeight: "bold",
},

overTextPinned: {
  color: "red",
  fontSize: 18,
  fontWeight: "bold",
  marginTop: -5,
  },

  totalRow: {
  flexDirection: "row",
  alignItems: "center",
  justifyContent: "center",
  marginTop: 60,
  gap: 10,
  zIndex: 20,
  position: "relative",
  height: 60,
  marginBottom: 30,
},
  searchWrapper: {
    width: "100%",
    alignItems: "center",
    marginTop: 10,
    marginBottom: 10,
  },
  searchBar: {
    width: 340,
    height: 40,
    maxWidth: 500,
    borderColor: "#ccc",
    borderWidth: 1,
    borderRadius: 10,
    paddingHorizontal: 12,
    backgroundColor: "white",
    color: "black",
    fontSize: 16,
  },
  categoryScroll: {
    marginBottom: 10,
    maxHeight: 40,
  },
  categoryButton: {
    backgroundColor: "#ddd",
    paddingVertical: 6,
    paddingHorizontal: 14,
    borderRadius: 20,
    marginHorizontal: 5,
  },
  Click: {
    fontSize: 20,
    color: "black",
    textAlign: "left",
    fontStyle: "italic",
    fontWeight: 'bold'
  },
  categorySelected: {
    backgroundColor: "#007aff",
  },
  categoryText: {
    fontSize: 14,
    fontWeight: "bold",
    color: "black",
  },
  priceFilterBox: {
    width: "100%",
    maxWidth: 500,
    alignItems: "center",
    marginBottom: 15,
  },
  priceInput: {
    width: 120,
    height: 40,
    borderColor: "#ccc",
    borderWidth: 1,
    borderRadius: 10,
    paddingHorizontal: 12,
    backgroundColor: "white",
    color: "black",
    fontSize: 16,
    marginBottom: 10,
  },
  filterButton: {
    backgroundColor: "#063E68",
    borderColor: '#ccc',
    borderWidth: 1,
    borderRadius: 8,
    paddingVertical: 10,
    paddingHorizontal: 20,
    marginBottom: 10,
  },
  buttonText: {
    color: "white",
    fontWeight: "bold",
    fontSize: 16,
  },
  totalPrice: {
    fontSize: 20,
    fontWeight: "bold",
    marginBottom: 10,
    color: "black",
  },
  grid: {
    flexDirection: "row",
    flexWrap: "wrap",
    justifyContent: "center",
  },
  itemBox: {
    alignItems: "center",
    margin: 10,
    width: 160,
    height: 230,
    backgroundColor: 'rgba(255, 255, 255, 0.9)',
    borderRadius: 10,
  },
  imagePlaceholder: {
    width: 155,
    height: 155,
    backgroundColor: "#ddd",
    borderRadius: 10,
    marginBottom: 5,
  },
  itemText: {
    fontSize: 16,
    fontWeight: "bold",
    color: "black",
    textAlign: "center",
  },
  itemImage: {
    width: 155,
    height: 155,
    borderRadius: 10,
    marginBottom: 5,
    resizeMode: "contain",
  },
  toast: {
    position: "absolute",
    width: 150,
    height: 50,
    bottom: 20,
    alignSelf: "center",
    backgroundColor: "#1c37b0",
    padding: 15,
    borderRadius: 8,
    zIndex: 20,
  },
  toastText: {
    color: "white",
    textAlign: "center",
    fontWeight: "bold",
  },
});
