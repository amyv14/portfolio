import React, { createContext, useContext, useState, ReactNode } from 'react';

// define the structure of an item in the cart
type Item = {
  id: number;
  name: string;
  price: number;
};

// define the context type for the cart
type CartContextType = {
  cart: Item[];
  addToCart: (item: Item) => void;
  removeFromCart: (id: number) => void;
};

// create context for cart
const CartContext = createContext<CartContextType | undefined>(undefined);

// cart provider component to wrap the app and provide cart state
export const CartProvider = ({ children }: { children: ReactNode }) => {
  const [cart, setCart] = useState<Item[]>([]);

  // function to add an item to the cart
  const addToCart = (item: Item) => {
    setCart(prev => [...prev, item]);
  };

  // function to remove an item from the cart by id
  const removeFromCart = (id: number) => {
    let removed = false;
    setCart(prev =>
      prev.filter(item => {
        if (!removed && item.id === id) {
          removed = true;
          return false; 
        }
        return true; 
      })
    );
  };

  // return the CartProvider with context value
  return (
    <CartContext.Provider value={{ cart, addToCart, removeFromCart }}>
      {children}
    </CartContext.Provider>
  );
};

// custom hook to use cart context
export const useCart = () => {
  const context = useContext(CartContext);
  if (!context) {
    throw new Error('useCart must be used within a CartProvider');
  }
  return context;
};
